# IvyDB: Key Value Store
Aiming to get my hands dirtier and implement a Key-Value Store (KV Store) utilizing the Log-Structured Merge-Tree (LSM Tree) as the underlying storage engine

You can read about LSM Trees in DBMSs, i recommend reading chapter 3 of "Designing Data Intensive Applications" [https://github.com/lafengnan/ebooks-1/blob/master/Designing%20Data%20Intensive%20Applications.pdf]

## Implementation details

SSTables: IvyDB implements SSTables for on-disk storage, providing a sorted representation of key-value pairs for efficient retrieval.

Levels and Level Compaction: IvyDB organizes SSTables into levels, with each level containing SSTables of increasing size. It performs level compaction to merge SSTables across levels.

Write Ahead Log (WAL): IvyDB includes a Write Ahead Log for durability, ensuring that data modifications are logged before being written to disk to prevent data loss in case of system crashes.

Recovery: In the event of a system crash, IvyDB has the ability to do recovery using the Write Ahead Log to restore the database to a consistent state.

Memtable: IvyDB utilizes a Memtable just with ```std::map``` (Red-black Tree)


Some of the data structures, such as the sparse index and custom comparators, were implemented naively for simplicity.
Leaving optimization and the more challenging implemenation after learning rust and follow skyzh's mini-lsm tutorial.
