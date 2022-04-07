#include <gtest/gtest.h>
#include "../src/tsdb.h"

TEST(Summary,DBCtor)
{
    TSDB();
}

TEST(Summary, CalcStartTime)
{
    Time t = 1649282091000; // Wed, 06 Apr 2022 21:54:51 GMT
    Time min1 = Bar::CalcStartTime(t, Bar::Resolution::BARS_1MIN);
    Time min5 = Bar::CalcStartTime(t, Bar::Resolution::BARS_5MIN);
    Time min15 = Bar::CalcStartTime(t, Bar::Resolution::BARS_15MIN);
    EXPECT_EQ( min1 , 1649282040000 ); // 21:54:00
    EXPECT_EQ( min5 , 1649281800000 ); // 21:50:00
    EXPECT_EQ( min15, 1649281500000 ); // 21:45:00
    // a 1 minute bar with a start time of 21:54 should have an end time of 21:54:59.9999
    Bar b(Bar::CalcStartTime(t, Bar::Resolution::BARS_1MIN), Bar::Resolution::BARS_1MIN);
    EXPECT_EQ( b.end, 1649282099999);
}

TEST(Summary, AddTick)
{
    TSDB db;
    db.AddResolution(Bar::Resolution::BARS_1MIN);
    db.AddResolution(Bar::Resolution::BARS_5MIN);
    // 100 ticks, 1 second apart
    for(int i = 0; i < 100; i++)
    {
        Tick t;
        t.price = 1 + ((double)i) / 100;
        t.timestamp = (i+1) * 1000;
        t.volume = i;
        db.AddTick(t);
    }
    auto collection = db.GetLast(10, Bar::Resolution::BARS_1MIN);
    EXPECT_EQ(collection.size(), 2);
    auto b = collection[0];
    EXPECT_EQ( (double)b->open, 1.5899); // <- note rounding problem
    EXPECT_EQ( (double)b->low, 1.5899);
    EXPECT_EQ( (double)b->close, 1.99);
    EXPECT_EQ( (double)b->high, 1.99);
    EXPECT_EQ( (double)b->volume, 3239 );
    EXPECT_EQ( b->start, 60000);
    EXPECT_EQ( b->end, b->start - 1 + std::chrono::milliseconds( std::chrono::minutes(1) ).count() );
    b = collection[1];
    EXPECT_EQ( (double)b->open, 1.0 );
    EXPECT_EQ( (double)b->low,  1.0 );
    EXPECT_EQ( (double)b->close, 1.58 );
    EXPECT_EQ( (double)b->high, 1.58 );
    EXPECT_EQ( (double)b->volume, 1711 );
    EXPECT_EQ( b->start, 0 );
    EXPECT_EQ( b->end, 59999 );
    collection = db.GetLast(10, Bar::Resolution::BARS_5MIN);
    EXPECT_EQ(collection.size(), 1);
    b = collection[0];
    EXPECT_EQ( (double)b->open, 1.0 );
    EXPECT_EQ( (double)b->low, 1.0 );
    EXPECT_EQ( (double)b->high, 1.99 );
    EXPECT_EQ( (double)b->close, 1.99 );
    EXPECT_EQ( (double)b->volume, 4950 );
    EXPECT_EQ( b->start, 0 );
    EXPECT_EQ( b->end, 299999 );
}