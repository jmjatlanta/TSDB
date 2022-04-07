#include "tsdb.h"

#include <chrono>

Bar::Bar(const Time& start, Bar::Resolution res) : start(start), resolution(res)
{
    end = GetEnd();
}

bool Bar::AddTick(const Tick& in)
{
    if (in.timestamp < this->start || in.timestamp > this->end )
        return false;
    if (firstTick == 0 || firstTick > in.timestamp)
    {
        firstTick = in.timestamp;
        open = in.price;
    }
    if (lastTick < in.timestamp)
    {
        lastTick = in.timestamp;
        close = in.price;
    }
    if (low == Amount((uint64_t)0) || in.price < low)
        low = in.price;
    if (in.price > high)
        high = in.price;
    volume += in.volume;
    return true;
}

Time Bar::GetStart() { return start; }

Time Bar::GetEnd()
{
    return start + ResolutionSpan() - 1;
}

Time Bar::ResolutionSpan()
{
    return ResolutionSpan(resolution);
}

Time Bar::ResolutionSpan(Bar::Resolution res)
{
    switch(res)
    {
        case(Resolution::BARS_1MIN):
            return std::chrono::milliseconds( std::chrono::minutes(1) ).count();
        case(Resolution::BARS_5MIN):
            return std::chrono::milliseconds( std::chrono::minutes(5) ).count();
        case(Resolution::BARS_15MIN):
            return std::chrono::milliseconds( std::chrono::minutes(15) ).count();
        case(Resolution::BARS_30MIN):
            return std::chrono::milliseconds( std::chrono::minutes(30) ).count();
        case(Resolution::BARS_1HR):
            return std::chrono::milliseconds( std::chrono::hours(1) ).count();
        case(Resolution::BARS_1DAY):
            return std::chrono::milliseconds( std::chrono::hours(24) ).count();
        case(Resolution::BARS_1WK):
            return std::chrono::milliseconds( std::chrono::hours(24 * 7) ).count();
        case(Resolution::BARS_1MO):
            return std::chrono::milliseconds( std::chrono::hours(24 * 30) ).count();
    }
    return 0;
}

Time Bar::CalcStartTime(const Time& timestamp, Bar::Resolution res)
{
    Time msPer = Bar::ResolutionSpan(res);
    return timestamp - (timestamp % msPer);
}

bool TSDB::AddBar(const Bar& in)
{
    // TODO: Find the start time and add ohlcv to the collection
    return false;
}

bool TSDB::AddTick(const Tick& in)
{
    ticks.insert(in);
    for( auto &coll : bars )
    {
        const Bar::Resolution res = coll.first;
        std::shared_ptr<Bar> b = GetBar(in.timestamp, coll.second);
        if (b == nullptr)
        {
            Time startTime = Bar::CalcStartTime( in.timestamp, res);
            coll.second.push_back( std::make_shared<Bar>(startTime, res) );
            b = coll.second.back();
        }
        b->AddTick(in);
    }
    return true;
}

bool TSDB::AddResolution(Bar::Resolution res)
{
    const auto itr = bars.find(res);
    if (itr != bars.end())
        return false;
    bars[res] = std::vector<std::shared_ptr<Bar>>();
    return true;
}

std::shared_ptr<Bar> TSDB::GetBar(const Time& timestamp, const std::vector<std::shared_ptr<Bar>>& coll)
{
    std::shared_ptr<Bar> retVal = nullptr;
    for(auto b : coll)
    {
        if (b->start <= timestamp && b->end >= timestamp)
            return b;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Bar>> TSDB::GetLast(uint32_t qty, Bar::Resolution resolution)
{
    std::vector<std::shared_ptr<Bar>>& coll = bars[resolution];
    std::vector<std::shared_ptr<Bar>> retVal;
    for(auto itr = coll.rbegin(); itr != coll.rend() && qty > 0; itr++)
    {
        retVal.push_back( (*itr) );
        qty--;
    }
    return retVal;
}

bool operator<(const Value& lhs, const Value& rhs)
{
    return lhs.data < rhs.data;
}
bool operator>(const Value& lhs, const Value& rhs) { return rhs < lhs; }
bool operator==(const Value& lhs, const Value& rhs) { return lhs.data == rhs.data; }
bool operator<(const Value& lhs, double rhs) { return lhs < Value(rhs, lhs.prec); }
bool operator==(const Value& lhs, double rhs) { return lhs == Value(rhs, lhs.prec); }

bool operator<(const Tick& lhs, const Tick& rhs)
{
    return lhs.timestamp < rhs.timestamp;
}
