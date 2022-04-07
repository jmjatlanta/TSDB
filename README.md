# TSDB
A simple time series database

The idea here is to store data coming in from Interactive Brokers in a fast and efficient manner.

Interactive Brokers tick data is hard to decipher. While last price seems accurate, there are many duplicates, and size is always questionable. Bar data seems to be more accurate. So during the building of a 1 minute bar, I should rely on tick data. After the minute, rely on bar data.

When building a 5 minute bar, for instance, things could get complicated. Tick data for the previous minute should be replaced by the 1 minute bar data. New tick data should be added to the bar.