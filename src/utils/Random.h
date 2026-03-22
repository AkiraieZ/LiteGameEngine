#pragma once

#include <random>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace LGE {

class Random {
public:
    static void init() {
        auto seed = static_cast<unsigned int>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        );
        s_rng.seed(seed);
    }

    static void init(unsigned int seed) {
        s_rng.seed(seed);
    }

    static int nextInt() {
        std::uniform_int_distribution<int> dist;
        return dist(s_rng);
    }

    static int nextInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(s_rng);
    }

    static int nextInt(int max) {
        return nextInt(0, max);
    }

    static float nextFloat() {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(s_rng);
    }

    static float nextFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(s_rng);
    }

    static float nextFloat(float max) {
        return nextFloat(0.0f, max);
    }

    static double nextDouble() {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(s_rng);
    }

    static double nextDouble(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(s_rng);
    }

    static double nextDouble(double max) {
        return nextDouble(0.0, max);
    }

    static bool nextBool() {
        std::bernoulli_distribution dist(0.5);
        return dist(s_rng);
    }

    static bool nextBool(double probability) {
        std::bernoulli_distribution dist(probability);
        return dist(s_rng);
    }

    static char nextChar() {
        return static_cast<char>(nextInt(32, 126));
    }

    static char nextLetter() {
        if (nextBool()) {
            return static_cast<char>(nextInt('A', 'Z'));
        } else {
            return static_cast<char>(nextInt('a', 'z'));
        }
    }

    static char nextDigit() {
        return static_cast<char>(nextInt('0', '9'));
    }

    static char nextLetterOrDigit() {
        if (nextBool()) {
            return nextLetter();
        } else {
            return nextDigit();
        }
    }

    static std::string nextString(size_t length) {
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; i++) {
            result += nextChar();
        }
        return result;
    }

    static std::string nextLetters(size_t length) {
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; i++) {
            result += nextLetter();
        }
        return result;
    }

    static std::string nextDigits(size_t length) {
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; i++) {
            result += nextDigit();
        }
        return result;
    }

    static std::string nextLettersAndDigits(size_t length) {
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; i++) {
            result += nextLetterOrDigit();
        }
        return result;
    }

    static std::string nextString(const std::string& charset, size_t length) {
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; i++) {
            result += charset[nextInt(static_cast<int>(charset.size()) - 1)];
        }
        return result;
    }

    template<typename T>
    static T& element(std::vector<T>& vec) {
        return vec[nextInt(static_cast<int>(vec.size()) - 1)];
    }

    template<typename T>
    static const T& element(const std::vector<T>& vec) {
        return vec[nextInt(static_cast<int>(vec.size()) - 1)];
    }

    template<typename T>
    static T elementOrDefault(const std::vector<T>& vec, const T& defaultValue) {
        if (vec.empty()) return defaultValue;
        return vec[nextInt(static_cast<int>(vec.size()) - 1)];
    }

    template<typename T>
    static void shuffle(std::vector<T>& vec) {
        std::shuffle(vec.begin(), vec.end(), s_rng);
    }

    template<typename T>
    static std::vector<T> shuffled(const std::vector<T>& vec) {
        std::vector<T> result = vec;
        shuffle(result);
        return result;
    }

    template<typename T>
    static std::vector<T> sample(const std::vector<T>& vec, size_t count) {
        std::vector<T> result;
        result.reserve(std::min(count, vec.size()));
        std::vector<size_t> indices(vec.size());
        for (size_t i = 0; i < vec.size(); i++) {
            indices[i] = i;
        }
        shuffle(indices);
        for (size_t i = 0; i < count && i < indices.size(); i++) {
            result.push_back(vec[indices[i]]);
        }
        return result;
    }

    template<typename T>
    static std::vector<T> choices(const std::vector<T>& vec, size_t count) {
        std::vector<T> result;
        result.reserve(count);
        for (size_t i = 0; i < count; i++) {
            result.push_back(element(vec));
        }
        return result;
    }

    static double normal(double mean = 0.0, double stddev = 1.0) {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(s_rng);
    }

    static float normal(float mean = 0.0f, float stddev = 1.0f) {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(s_rng);
    }

    static int poisson(double mean) {
        std::poisson_distribution<int> dist(mean);
        return dist(s_rng);
    }

    static double exponential(double lambda) {
        std::exponential_distribution<double> dist(lambda);
        return dist(s_rng);
    }

    static double gamma(double alpha, double beta) {
        std::gamma_distribution<double> dist(alpha, beta);
        return dist(s_rng);
    }

    static double weibull(double a, double b) {
        std::weibull_distribution<double> dist(a, b);
        return dist(s_rng);
    }

    static double extremeValue(double a, double b) {
        std::extreme_value_distribution<double> dist(a, b);
        return dist(s_rng);
    }

    static int binomial(int t, double p) {
        std::binomial_distribution<int> dist(t, p);
        return dist(s_rng);
    }

    static int geometric(double p) {
        std::geometric_distribution<int> dist(p);
        return dist(s_rng);
    }

    static int negativeBinomial(int k, double p) {
        std::negative_binomial_distribution<int> dist(k, p);
        return dist(s_rng);
    }

    static int discrete(const std::vector<double>& weights) {
        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        return dist(s_rng);
    }

    static int discrete(const std::vector<int>& weights) {
        std::vector<double> dweights(weights.begin(), weights.end());
        return discrete(dweights);
    }

    template<typename T>
    static T weightedChoice(const std::vector<T>& items, const std::vector<double>& weights) {
        size_t index = static_cast<size_t>(discrete(weights));
        return items[std::min(index, items.size() - 1)];
    }

    template<typename T>
    static T weightedChoice(const std::vector<T>& items, const std::vector<int>& weights) {
        std::vector<double> dweights(weights.begin(), weights.end());
        return weightedChoice(items, dweights);
    }

    static double range(double min, double max) {
        return nextDouble(min, max);
    }

    static float range(float min, float max) {
        return nextFloat(min, max);
    }

    static int range(int min, int max) {
        return nextInt(min, max);
    }

    static double angle() {
        return nextDouble(0.0, 2.0 * 3.14159265358979323846);
    }

    static float angleF() {
        return nextFloat(0.0f, 2.0f * 3.14159265358979323846f);
    }

    static double degrees() {
        return nextDouble(0.0, 360.0);
    }

    static float degreesF() {
        return nextFloat(0.0f, 360.0f);
    }

    static double sign() {
        return nextBool() ? 1.0 : -1.0;
    }

    static float signF() {
        return nextBool() ? 1.0f : -1.0f;
    }

    static int signInt() {
        return nextBool() ? 1 : -1;
    }

    static double gaussian(double mean, double stddev) {
        return normal(mean, stddev);
    }

    static float gaussian(float mean, float stddev) {
        return normal(mean, stddev);
    }

    static double clamp(double value, double min, double max) {
        return std::min(std::max(value, min), max);
    }

    static float clamp(float value, float min, float max) {
        return std::min(std::max(value, min), max);
    }

    static int clamp(int value, int min, int max) {
        return std::min(std::max(value, min), max);
    }

    static double lerp(double a, double b, double t) {
        return a + (b - a) * t;
    }

    static float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    static double smoothstep(double edge0, double edge1, double x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
        return x * x * (3 - 2 * x);
    }

    static float smoothstep(float edge0, float edge1, float x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * (3.0f - 2.0f * x);
    }

    static double smootherstep(double edge0, double edge1, double x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
        return x * x * x * (x * (x * 6 - 15) + 10);
    }

    static float smootherstep(float edge0, float edge1, float x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * x * (x * (x * 6.0f - 15.0f) + 10.0f);
    }

    static double triangular(double min, double mode, double max) {
        double u = nextDouble();
        double range = max - min;
        double modePos = (mode - min) / range;
        
        if (u < modePos) {
            return min + std::sqrt(u * range * (mode - min));
        } else {
            return max - std::sqrt((1 - u) * range * (max - mode));
        }
    }

    static float triangular(float min, float mode, float max) {
        float u = nextFloat();
        float range = max - min;
        float modePos = (mode - min) / range;
        
        if (u < modePos) {
            return min + std::sqrt(u * range * (mode - min));
        } else {
            return max - std::sqrt((1 - u) * range * (max - mode));
        }
    }

    static double cauchy(double location, double scale) {
        double u = nextDouble(0.0001, 0.9999);
        return location + scale * std::tan(3.14159265358979323846 * (u - 0.5));
    }

    static float cauchy(float location, float scale) {
        float u = nextFloat(0.0001f, 0.9999f);
        return location + scale * std::tan(3.14159265358979323846f * (u - 0.5f));
    }

    static double laplace(double location, double scale) {
        double u = nextDouble(-0.5, 0.5);
        return location - scale * (u > 0 ? 1 : -1) * std::log(1 - 2 * std::abs(u));
    }

    static float laplace(float location, float scale) {
        float u = nextFloat(-0.5f, 0.5f);
        return location - scale * (u > 0 ? 1 : -1) * std::log(1 - 2 * std::abs(u));
    }

    static double logistic(double location, double scale) {
        double u = nextDouble(0.0001, 0.9999);
        return location + scale * std::log(u / (1 - u));
    }

    static float logistic(float location, float scale) {
        float u = nextFloat(0.0001f, 0.9999f);
        return location + scale * std::log(u / (1 - u));
    }

    static double rayleigh(double sigma) {
        double u = nextDouble(0.0001, 0.9999);
        return sigma * std::sqrt(-2 * std::log(u));
    }

    static float rayleigh(float sigma) {
        float u = nextFloat(0.0001f, 0.9999f);
        return sigma * std::sqrt(-2.0f * std::log(u));
    }

    static double pareto(double scale, double shape) {
        double u = nextDouble(0.0001, 0.9999);
        return scale / std::pow(u, 1.0 / shape);
    }

    static float pareto(float scale, float shape) {
        float u = nextFloat(0.0001f, 0.9999f);
        return scale / std::pow(u, 1.0f / shape);
    }

    static std::string uuid() {
        static const char* hex = "0123456789abcdef";
        std::string result;
        result.reserve(36);
        
        for (int i = 0; i < 8; i++) result += hex[nextInt(15)];
        result += '-';
        for (int i = 0; i < 4; i++) result += hex[nextInt(15)];
        result += '-';
        result += '4';
        for (int i = 0; i < 3; i++) result += hex[nextInt(15)];
        result += '-';
        result += hex[nextInt(8, 11)];
        for (int i = 0; i < 3; i++) result += hex[nextInt(15)];
        result += '-';
        for (int i = 0; i < 12; i++) result += hex[nextInt(15)];
        
        return result;
    }

    static std::string hex(size_t length) {
        static const char* hex = "0123456789abcdef";
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; i++) {
            result += hex[nextInt(15)];
        }
        return result;
    }

    static std::string base64(size_t length) {
        static const char* chars = 
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string result;
        result.reserve(length);
        for (size_t i = 0; i < length; i++) {
            result += chars[nextInt(63)];
        }
        return result;
    }

    template<typename T>
    static T dice(int sides, int count = 1) {
        T result = 0;
        for (int i = 0; i < count; i++) {
            result += static_cast<T>(nextInt(1, sides));
        }
        return result;
    }

    static int d4(int count = 1) { return dice<int>(4, count); }
    static int d6(int count = 1) { return dice<int>(6, count); }
    static int d8(int count = 1) { return dice<int>(8, count); }
    static int d10(int count = 1) { return dice<int>(10, count); }
    static int d12(int count = 1) { return dice<int>(12, count); }
    static int d20(int count = 1) { return dice<int>(20, count); }
    static int d100(int count = 1) { return dice<int>(100, count); }

    static bool chance(int percent) {
        return nextInt(100) < percent;
    }

    static bool chance(double probability) {
        return nextDouble() < probability;
    }

    static bool oneIn(int n) {
        return nextInt(n - 1) == 0;
    }

private:
    static std::mt19937 s_rng;
};

}
