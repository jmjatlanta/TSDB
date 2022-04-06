#include <gtest/gtest.h>
#include "../src/tsdb.h"

TEST(Summary,DBCtor)
{
    TSDB();
}

TEST(Summary, AddTick)
{
    TSDB db;
    for(int i = 0; i < 100; i++)
    {
        Tick t;
        t.price = 1 + ((double)i) / 100;
        t.timestamp = i+1;
        t.volume = i;
        db.AddTick(t);
    }
    auto collection = db.GetLast(1, Bar::Resolution::BARS_5MIN);
    EXPECT_EQ(collection.size(), 1);
    auto b = collection[0];
    EXPECT_EQ( (double)b.open, 1.0);
    EXPECT_EQ( (double)b.low, 1.0);
    EXPECT_EQ( (double)b.close, 1.99);
    EXPECT_EQ( (double)b.high, 1.99);
    EXPECT_EQ( b.start, 1);
    EXPECT_EQ( b.end, std::chrono::milliseconds( std::chrono::minutes(5) ).count() );
}