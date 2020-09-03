#include "hzpch.h"

namespace SpiritEngine
{
    class Random
    {
    public:
        int Range(int minValue, int maxValue)
        {
            // Always set a seed value.
            srand((unsigned int)time(NULL));
            int RandomValue = (rand() % maxValue) + minValue;
            return RandomValue;
        }

        /*float Range(float minValue, float maxValue)
        {
            // Always set a seed value.
            srand((unsigned int)time(NULL));
            float RandomValue = (rand() % maxValue) + minValue;
            return RandomValue;
        }*/

        int ZeroRange(int maxValue)
        {
            // Always set a seed value.
            srand((unsigned int)time(NULL));
            int RandomValue = rand() % maxValue;
            return RandomValue;
        }

        /*float ZeroRange(float maxValue)
        {
            // Always set a seed value.
            srand((unsigned int)time(NULL));
            float RandomValue = rand() % maxValue;
            return RandomValue;
        }*/

        int OneRange(int maxValue)
        {
            // Always set a seed value.
            srand((unsigned int)time(NULL));
            int RandomValue = (rand() % maxValue) + 1;
            return RandomValue;
        }

        /*float OneRange(float maxValue)
        {
            // Always set a seed value.
            srand((unsigned int)time(NULL));
            float RandomValue = (rand() % maxValue) + 1;
            return RandomValue;
        }*/
    };
}