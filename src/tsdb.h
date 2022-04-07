#include <set>
#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <memory>

#define AMOUNT_PRECISION 10000
#define VOLUME_PRECISION 1

typedef uint64_t Time;

class Value
{
public:
    Value(double in, uint32_t prec) : data(in * prec), prec(prec) { }
    Value(uint64_t in, uint32_t prec) : data(in), prec(prec) { }

    operator double() { return to_double(); }

    Value& operator+=(const Value& rhs) { data += rhs.data; return *this; }

    friend bool operator<(const Value& lhs, const Value& rhs);
    friend bool operator>(const Value& lhs, const Value& rhs);
    friend bool operator==(const Value& lhs, const Value& rhs);
    friend bool operator<(const Value& lhs, double rhs);
    friend bool operator>(const Value& lhs, double rhs);
    friend bool operator==(const Value& lhs, double rhs);

    uint64_t data;
    double to_double() const
    {
        return ((double)data) / prec;
    }
    std::string to_string() const
    {
        return std::to_string(to_double());
    }
private:
    uint32_t prec;
};


class Amount : public Value
{
public:
    Amount() : Amount(0.0) {}
    Amount(double in) : Value(in, AMOUNT_PRECISION) {}
    Amount(uint64_t in) : Value(in, AMOUNT_PRECISION) {}
};

class Volume : public Value
{
public:
    Volume() : Value(0.0, VOLUME_PRECISION) {}
    Volume(uint64_t in) : Value(in, VOLUME_PRECISION) {}
};

class Tick
{
public:
    Tick() {};
    Tick(const Amount& price, const Volume& vol, const Time& timestamp) 
            : price(price), volume(vol), timestamp(timestamp){}
    Amount price;
    Volume volume;
    Time timestamp;
    friend bool operator<(const Tick& lhs, const Tick& rhs);
    inline bool operator<(const Tick& other)
    {
        return timestamp < other.timestamp;
    }
};


class Bar
{
public:
    enum Resolution
    {
        BARS_1MIN,
        BARS_5MIN,
        BARS_15MIN,
        BARS_30MIN,
        BARS_1HR,
        BARS_1DAY,
        BARS_1WK,
        BARS_1MO
    };

    Bar(const Time& start, Resolution res);

    Time start = 0;
    Time end = 0;
    Time firstTick = 0;
    Time lastTick = 0;
    Amount open;
    Amount high;
    Amount low;
    Amount close;
    Volume volume;
    /***
     * @brief add a tick to this bar
     * @note open will be inaccurate unless entire bar built via ticks
     * @param in the tick
     * @returns true if added
     */
    bool AddTick(const Tick& in);
    static Time ResolutionSpan(Resolution res);
    /****
     * @param in the timestamp
     * @param res the resolution
     * @return the start time of the bar that contains this timestamp
     */
    static Time CalcStartTime(const Time& in, Bar::Resolution res);
private:
    /***
     * @return the start time of the bar
     */
    Time GetStart();
    /***
     * @return the end time of the bar
     */
    Time GetEnd();
    /***
     * @returns number of ms for this resolution
     */
    Time ResolutionSpan();
    Resolution resolution;
};

class TSDB
{
public:
    /***
     * @brief Add a (lowest resolution) bar to the database
     * @param in the bar to add
     * @returns true on success
     */
    bool AddBar(const Bar& in);
    /***
     * @brief Add a tick to the database
     * @param in the tick
     * @returns true on success
     */
    bool AddTick(const Tick& in);
    /******
     * @brief return the most recent (qty) bars
     * @param qty the maximum number of bars to return
     * @param resolution the timespan of 1 bar
     * @returns the bars
     */
    std::vector<std::shared_ptr<Bar>> GetLast(uint32_t qty, Bar::Resolution resolution);
    /*****
     * @brief keep track of a resolution
     * @param res the resolution
     * @return false if resolution was already there
     */
    bool AddResolution(Bar::Resolution res);
protected:
    std::multiset<Tick> ticks;
    std::map<Bar::Resolution, std::vector<std::shared_ptr<Bar> > > bars;

    /****
     * @param timestamp the timestamp
     * @param coll the collection of bars (all of the same resolution)
     * @returns the bar that contains the timestamp
     */
    std::shared_ptr<Bar> GetBar(const Time& timestamp, const std::vector<std::shared_ptr<Bar>>& coll);
};