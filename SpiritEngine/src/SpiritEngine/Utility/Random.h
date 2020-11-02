#pragma once

#include <random>

namespace SpiritEngine
{
    class Random
    {
    public:
        /*
        * Returns a Random Real between min and max
        */
        template<typename T>
        static T Real(T min, T max)
        {
            SPIRIT_CORE_ASSERT(min < max, "min is >= max");

            std::uniform_real_distribution<T> dist(min, max);
            auto& mt = GetEngine();
            return dist(mt);
        }

        /*
        * Returns a Random Integer between min and max
        */
        template<typename T>
        static T Int(T min, T max)
        {
            SPIRIT_CORE_ASSERT(min < max, "min is >= max");

            std::uniform_int_distribution<T> dist(min, max);
            auto& mt = GetEngine();
            return dist(mt);
        }

        /*
        * Returns a Random bool value, either 'true' or 'false'
        */
        static bool Bool()
        {
            std::bernoulli_distribution dist(0.5f);
            auto& mt = GetEngine();
            return dist(mt);
        }

        template<typename T>
        static T ZeroRange(T maxValue)
        {
            // Always set a seed value.
            srand((unsigned T)time(NULL));
            T RandomValue = (T)((T)rand() % maxValue);
            return RandomValue;
        }

        template<typename T>
        static T OneRange(T maxValue)
        {
            // Always set a seed value.
            srand((unsigned T)time(NULL));
            T RandomValue = (T)((T)rand() % maxValue) + 1;
            return RandomValue;
        }

    private:
        static std::mt19937& GetEngine()
        {
            static std::random_device seedGen;
            static std::mt19937 engine(seedGen());
            return engine;
        }
    };
}