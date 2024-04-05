#pragma once

#include <memory>
#include <type_traits>
#include "UInt.hpp"

namespace fpga::utils {

    // 对于硬件模块来说
    // 输入信号总是由外部模块提供（必然不持有）
    // 输出信号可能持有，也可能不持有

    // 故 In  只能被转换为 const T&
    // 而 Out 可以被转换为       T&

    // 所有的中间 Bundle 的成员都必须用 Wire<> 包裹

    // 信号的类型：
    // 1. 基本类型（POD）或者 指针类型 或者 UInt<w>（w <= 64）
    //    此时直接保存值
    // 2. 非基本类型：保存 std::shared_ptr

    template <typename T>
    static constexpr bool is_uint = false;

    template <size_t w> requires (w <= 64)
    static constexpr bool is_uint<UInt<w>> = true;

    // 下面首先判断是否为基本类型
    template <typename T>
    static constexpr bool is_direct_type = std::is_fundamental_v<T> || std::is_same_v<T, const char*> || is_uint<T>;

    // 统一模板类
    // R 只能为 const T& 或 T&
    // 1. R = const T& 时，表示输入信号
    // 2. R = T& 时，表示输出信号
    template <typename T, typename R>
    concept signal_type = std::is_same_v<R, const T&> || std::is_same_v<R, T&>;

    // 模板类的基本定义
    template <typename T, typename R, bool D = is_direct_type<T>>
    class Signal;

    // 如果是基本类型，则直接存储值
    template <typename T, typename R> requires signal_type<T, R>
    class Signal<T, R, true> {
    public:
        using value_type = T;
        using reference_type = R;
        constexpr static bool is_input_signal = std::is_same_v<R, const T&>;
        constexpr static bool is_output_signal = std::is_same_v<R, T&>;

        T value;

        // 转换到 R
        constexpr operator R() {
            return value;
        }

        // 转换到 const T&
        constexpr operator const T& () const {
            return value;
        }

        // 赋值
        // 对于基本类型，值类型和引用类型没有区别
        constexpr Signal& operator=(const T& v) {
            value = v;
            return *this;
        }

        template <typename R2>
        constexpr Signal& operator=(const Signal<T, R2>& w) {
            value = w.value;
            return *this;
        }

        // 转换到 R（允许初始化）
        constexpr R get() {
            return value;
        }

        // 即使是 const 也可以调用 get（允许初始化）
        constexpr R get() const {
            return const_cast<Signal*>(this)->get();
        }
    };

    // 如果是非基本类型，则存储指针
    template <typename T, typename R> requires signal_type<T, R>
    class Signal<T, R, false> {
    public:
        using value_type = T;
        using reference_type = R;
        constexpr static bool is_input_signal = std::is_same_v<R, const T&>;
        constexpr static bool is_output_signal = std::is_same_v<R, T&>;

        std::shared_ptr<T> ptr = nullptr;

        // 转换到 R
        constexpr operator R() {
            return get();
        }

        // 转换到 const T&
        constexpr operator const T& () const {
            return get();
        }

        constexpr operator bool() const {
            return ptr != nullptr;
        }

        // 赋值
        // 对于基本类型，值类型和引用类型没有区别
        constexpr Signal& operator=(const T& v) {
            ptr = std::make_shared<T>(v);
            return *this;
        }

        template <typename R2>
        constexpr Signal& operator=(const Signal<T, R2>& w) {
            if (w) {
                ptr = w.ptr;
            }
            else {
                ptr = nullptr;
            }
            return *this;
        }

        constexpr Signal& operator=(std::shared_ptr<T>&& p) {
            ptr = std::move(p);
            return *this;
        }

        constexpr Signal& operator=(std::nullptr_t) {
            ptr = nullptr;
            return *this;
        }

        // 转换到 R（如果为空会强制初始化）
        constexpr R get() {
            if (ptr == nullptr) {
                ptr = std::make_shared<T>();
            }
            return *ptr;
        }

        // 转换到 R（如果为空会强制初始化）
        constexpr const T& get() const {
            if (ptr == nullptr) {
                ptr = std::make_shared<T>();
            }
            return *ptr;
        }

        constexpr std::shared_ptr<T> get_shared_ptr() const {
            return ptr;
        }
    };

    template <typename T>
    using In = Signal<T, const T&>;

    template <typename T>
    using Out = Signal<T, T&>;

    template <typename T>
    using Wire = Signal<T, T&>;
}