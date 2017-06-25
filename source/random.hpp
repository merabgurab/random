﻿#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>
#include <type_traits>

namespace effolkronium {
    /**
    * \brief Base template class for random
    * \param Engine A random engine with interface like in the std::mt19937
    */
    template<typename Engine>
    class basic_random_static {
    public:
        /// Type of used random number engine
        using engine_type = Engine;

        /// Key type for getting common_type numbers or objects
        struct common { };

        /// True if type T is applicable by the std::uniform_int_distribution
        template<typename T>
        struct is_uniform_int {
            static constexpr bool value =
                   std::is_same<T,              short>::value
                || std::is_same<T,                int>::value
                || std::is_same<T,               long>::value
                || std::is_same<T,          long long>::value
                || std::is_same<T,     unsigned short>::value
                || std::is_same<T,       unsigned int>::value
                || std::is_same<T,      unsigned long>::value
                || std::is_same<T, unsigned long long>::value;
        };

        /// True if type T is applicable by a std::uniform_real_distribution
        template<typename T>
        struct is_uniform_real {
            static constexpr bool value =
                   std::is_same<T,       float>::value
                || std::is_same<T,      double>::value
                || std::is_same<T, long double>::value;
        };

        /// True if type T is plain byte
        template<typename T>
        struct is_byte {
            static constexpr bool value =
                   std::is_same<T,   signed char>::value
                || std::is_same<T, unsigned char>::value;
        };

        /// True if type T is plain number type
        template<typename T>
        struct is_supported_number {
            static constexpr bool value =
                   is_byte        <T>::value
                || is_uniform_real<T>::value
                || is_uniform_int <T>::value;
        };

        /**
        * \brief Generate a random integer number in a [from, to] range
        *        by std::uniform_int_distribution
        * \param from The first limit number of a random range
        * \param to The second limit number of a random range
        * \return A random integer number in a [from, to] range
        * \note Allow both: 'from' <= 'to' and 'from' >= 'to'
		* \note Prevent implicit type conversion
        */
        template<typename A>
        static typename std::enable_if<is_uniform_int<A>::value
            , A>::type get( A from, A to ) noexcept {
            if( from < to ) // Allow range from higher to lower
                return std::uniform_int_distribution<A>{ from, to }( engine );
            return std::uniform_int_distribution<A>{ to, from }( engine );
        }

        /**
        * \brief Generate a random real number in a [from, to] range
        *        by std::uniform_real_distribution
        * \param from The first limit number of a random range
        * \param to The second limit number of a random range
        * \return A random real number in a [from, to] range
        * \note Allow both: 'from' <= 'to' and 'from' >= 'to'
        * \note Prevent implicit type conversion
        */
        template<typename A>
        static typename std::enable_if<is_uniform_real<A>::value
            , A>::type get( A from, A to ) noexcept {
            if( from < to ) // Allow range from higher to lower
                return std::uniform_real_distribution<A>{ from, to }( engine );
            return std::uniform_real_distribution<A>{ to, from }( engine );
        }

        /**
        * \brief Generate a random byte number in a [from, to] range
        * \param from The first limit number of a random range
        * \param to The second limit number of a random range
        * \return A random byte number in a [from, to] range
        * \note Allow both: 'from' <= 'to' and 'from' >= 'to'
        * \note Prevent implicit type conversion
        */
        template<typename A>
        static typename std::enable_if<is_byte<A>::value
            , A>::type get( A from, A to ) noexcept {
            // Choose between short and unsigned short for byte conversion
            using short_t = typename std::conditional<
                std::is_signed<A>::value, short, unsigned short>::type;

            return static_cast<A>( get( static_cast<short_t>( from ),
                                        static_cast<short_t>( to ) ) );
        }

        /**
        * \brief Generate a random common_type number in a [from, to] range
        * \param Key The Key type for this version of 'get' method
        *        Type should be '(THIS_TYPE)::common' struct
        * \param from The first limit number of a random range
        * \param to The second limit number of a random range
        * \return A random common_type number in a [from, to] range
        * \note Allow both: 'from' <= 'to' and 'from' >= 'to'
        * \note Allow implicit type conversion
        * \note Prevent implicit type conversion from singed to unsigned types
        *       Why? std::common_type<Unsigned, Signed> chooses unsigned value,
        *                 then Signed value will be converted to Unsigned value
        *                       which gives us a wrong range for random values.
        *                           https://stackoverflow.com/a/5416498/5734836
        */
        template<typename Key, typename A, typename B, 
                 typename C = typename std::common_type<A, B>::type>
        static typename std::enable_if<
               std::is_same<Key, common>::value
            && is_supported_number<A>::value
            && is_supported_number<B>::value
            // Prevent implicit type conversion from singed to unsigned types
            && std::is_signed<A>::value != std::is_unsigned<B>::value
            , C>::type get( A from, B to ) noexcept {
            return get( static_cast<C>( from ), static_cast<C>( to ) );
        }
    private:
        /// The random number engine
        static Engine engine;
    };

    template<typename Engine>
    Engine basic_random_static<Engine>::engine( std::random_device{ }( ) );

    /** 
    * \brief The basic static random alias based on a std::default_random_engine
    * \note It uses static methods API and data with static storage
    * \note Not thread safe but more prefomance
    */
    using random_static = basic_random_static<std::default_random_engine>;

} // namespace effolkronium

#endif // #ifndef RANDOM_HPP