#pragma once

#include "Common.h"

namespace cppe {

	template<typename T>
	class IEnumerator;

    namespace Enumeration {

        template<typename T>
		class IEnumeratorPromise
		{
		public:

			using value_type = std::remove_reference_t<T>;
			using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
			using pointer_type = value_type*;

			IEnumeratorPromise() = default;

			IEnumerator<T> get_return_object() noexcept;

			constexpr std::suspend_always initial_suspend() const noexcept { return {}; }
			constexpr std::suspend_always final_suspend() const noexcept { return {}; }

			template<
				typename U = T,
				std::enable_if_t<!std::is_rvalue_reference<U>::value, int> = 0>
			std::suspend_always yield_value(std::remove_reference_t<T>& value) noexcept
			{
				m_value = std::addressof(value);
				return {};
			}

			std::suspend_always yield_value(std::remove_reference_t<T>&& value) noexcept
			{
				m_value = std::addressof(value);
				return {};
			}

			void unhandled_exception()
			{
				m_exception = std::current_exception();
			}

			void return_void()
			{
			}

			reference_type value() const noexcept
			{
				return static_cast<reference_type>(*m_value);
			}

			// Don't allow any use of 'co_await' inside the IEnumerator coroutine.
			template<typename U>
			std::suspend_never await_transform(U&& value) = delete;

			void rethrow_if_exception()
			{
				if (m_exception)
				{
					std::rethrow_exception(m_exception);
				}
			}

		private:
			pointer_type m_value;
			std::exception_ptr m_exception;

		};

        struct IEnumeratorSentinel {};

        template<typename T>
		class IEnumeratorIterator
		{
			using coroutine_handle = std::coroutine_handle<IEnumeratorPromise<T>>;

		public:

			using iterator_category = std::input_iterator_tag;
			// What type should we use for counting elements of a potentially infinite sequence?
			using difference_type = std::ptrdiff_t;
			using value_type = typename IEnumeratorPromise<T>::value_type;
			using reference = typename IEnumeratorPromise<T>::reference_type;
			using pointer = typename IEnumeratorPromise<T>::pointer_type;

			// Iterator needs to be default-constructible to satisfy the Range concept.
			IEnumeratorIterator() noexcept
				: m_coroutine(nullptr)
			{}
			
			explicit IEnumeratorIterator(coroutine_handle coroutine) noexcept
				: m_coroutine(coroutine)
			{}

			friend bool operator==(const IEnumeratorIterator& it, IEnumeratorSentinel) noexcept
			{
				return !it.m_coroutine || it.m_coroutine.done();
			}

			friend bool operator!=(const IEnumeratorIterator& it, IEnumeratorSentinel s) noexcept
			{
				return !(it == s);
			}

			friend bool operator==(IEnumeratorSentinel s, const IEnumeratorIterator& it) noexcept
			{
				return (it == s);
			}

			friend bool operator!=(IEnumeratorSentinel s, const IEnumeratorIterator& it) noexcept
			{
				return it != s;
			}

			IEnumeratorIterator& operator++()
			{
				m_coroutine.resume();
				if (m_coroutine.done())
				{
					m_coroutine.promise().rethrow_if_exception();
				}

				return *this;
			}

			// Need to provide post-increment operator to implement the 'Range' concept.
			void operator++(int)
			{
				(void)operator++();
			}

			reference operator*() const noexcept
			{
				return m_coroutine.promise().value();
			}

			pointer operator->() const noexcept
			{
				return std::addressof(operator*());
			}

		private:

			coroutine_handle m_coroutine;
		};
    }

	template<typename T>
	class [[nodiscard]] IEnumerator
	{
	public:

		using promise_type = Enumeration::IEnumeratorPromise<T>;
		using iterator = Enumeration::IEnumeratorIterator<T>;

		IEnumerator() noexcept
			: m_coroutine(nullptr)
		{}

		IEnumerator(IEnumerator&& other) noexcept
			: m_coroutine(other.m_coroutine)
		{
			other.m_coroutine = nullptr;
		}

		IEnumerator(const IEnumerator& other) = delete;

		~IEnumerator()
		{
			if (m_coroutine)
			{
				m_coroutine.destroy();
			}
		}

		IEnumerator& operator=(IEnumerator other) noexcept
		{
			swap(other);
			return *this;
		}

		iterator begin()
		{
			if (m_coroutine)
			{
				m_coroutine.resume();
				if (m_coroutine.done())
				{
					m_coroutine.promise().rethrow_if_exception();
				}
			}

			return iterator{ m_coroutine };
		}

		Enumeration::IEnumeratorSentinel end() noexcept
		{
			return Enumeration::IEnumeratorSentinel{};
		}

		void swap(IEnumerator& other) noexcept
		{
			std::swap(m_coroutine, other.m_coroutine);
		}

	private:

		friend class Enumeration::IEnumeratorPromise<T>;

		explicit IEnumerator(std::coroutine_handle<promise_type> coroutine) noexcept
			: m_coroutine(coroutine)
		{}

		std::coroutine_handle<promise_type> m_coroutine;

	};

	template<typename T>
	void swap(IEnumerator<T>& a, IEnumerator<T>& b)
	{
		a.swap(b);
	}

	namespace Enumeration
	{
		template<typename T>
		IEnumerator<T> IEnumeratorPromise<T>::get_return_object() noexcept
		{
			using coroutine_handle = std::coroutine_handle<IEnumeratorPromise<T>>;
			return IEnumerator<T>{ coroutine_handle::from_promise(*this) };
		}
	}

	template<typename FUNC, typename T>
	IEnumerator<std::invoke_result_t<FUNC&, typename IEnumerator<T>::iterator::reference>> fmap(FUNC func, IEnumerator<T> source)
	{
		for (auto&& value : source)
		{
			co_yield std::invoke(func, static_cast<decltype(value)>(value));
		}
	}
}