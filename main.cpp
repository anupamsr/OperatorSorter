#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <map>

static std::map<std::chrono::system_clock::time_point, std::tm> o;
void printTime(const std::chrono::system_clock::time_point& tp)
{
    char buffer[80];

    std::strftime(buffer, 80, "%Y-%m-%d %H:%M", &o.at(tp));
    std::cout << buffer;
}

auto getTime(const std::string str)
{
    std::tm tm = {};

    tm.tm_year = 70;
    std::stringstream ss(str);

    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M");
    time_t t  = std::mktime(&tm);
    auto   tp = std::chrono::system_clock::from_time_t(t);

    o[tp] = tm;
    return tp;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Please specify input file as 1st argument" << std::endl;
        return 1;
    }

    std::ifstream ifs;

    ifs.open(argv[1], std::ios::in);
    if (!ifs.is_open())
    {
        std::cerr << "Error opening file " << argv[1] << std::endl;
        return 2;
    }

    std::string line;

    std::getline(ifs, line);
    size_t num_input = std::stoul(line);
    std::map<std::chrono::system_clock::time_point, size_t> m;
    size_t sum = 0;
    size_t max = 0;
    std::chrono::system_clock::time_point start, end;

    for (size_t i = 0; i < num_input; ++i)
    {
        std::string line;
        std::getline(ifs, line);
        auto strs  = split(line, ',');
        auto curr  = getTime(strs[0]);
        auto count = std::stoul(strs[1]);
        m[curr] = count;

        //        std::cout << "Adding ";
        //        printTime(curr);
        //        std::cout << " = " << m[curr] << std::endl;
        if (m.size() == 1)
        {
            sum   = count;
            max   = sum;
            start = curr;
            end   = curr + std::chrono::minutes(60);
            for (size_t i = 1; i < 60; ++i)
            {
                auto next = curr + std::chrono::minutes(i);
                m[next] = 0;
                std::tm t;
                auto    rawtime = std::chrono::system_clock::to_time_t(next);
                localtime_r(&rawtime, &t);
                o[next] = t;
            }
        }
        else
        {
            auto prev = curr - std::chrono::minutes(60);
            auto p_it = m.find(prev);
            if (p_it != m.end())
            {
                for (auto it = m.begin(); it != p_it;)
                {
                    //                    std::cout << "Removing ";
                    //                    printTime(it->first);
                    //                    std::cout << " = " << it->second << std::endl;
                    sum -= it->second;
                    m.erase(it++);
                }
            }

            sum += m[curr];
            if (sum > max)
            {
                max   = sum;
                start = curr - std::chrono::minutes(60);
                end   = curr;
            }
        }

        //        std::cout << "Current sum = " << sum << ", max = " << max << std::endl;
    }

    ifs.close();
    std::cout << max << std::endl;
    char buffer[80];

    std::strftime(buffer, 80, "%Y-%m-%d %H:%M", &o[start]);
    std::cout << buffer << ", ";
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M", &o[end]);
    std::cout << buffer;
    return 0;
}
