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
    return true;
}

Time Bar::GetStart() { return start; }

Time Bar::GetEnd()
{
    return start + ResolutionSpan() - 1;
}

Time Bar::ResolutionSpan()
{
    switch(resolution)
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

bool TSDB::AddBar(const Bar& in)
{
    // TODO: Find the start time and add ohlcv to the collection
    return false;
}

bool TSDB::AddTick(const Tick& in)
{
    ticks.insert(in);
    return true;
}

std::vector<Bar> TSDB::GetLast(uint32_t qty, Bar::Resolution resolution)
{
    std::vector<Bar> retVal( {Bar(1, Bar::Resolution::BARS_5MIN)} );
    Bar& b = retVal.back();
    for( auto& tick : ticks )
    {
        b.AddTick(tick);
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
