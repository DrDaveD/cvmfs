/**
 * This file is part of the CernVM File System.
 */

#include <gtest/gtest.h>

#include "testutil.h"

#include "catalog_counters.h"

namespace catalog {

class T_CatalogCounters : public ::testing::Test {
 protected:
  DeltaCounters GetFilledDeltaCounters() const {
    DeltaCounters d;
    d.self.regular_files   = 102;
    d.self.symlinks        = 10;
    d.self.specials        = 3;
    d.self.directories     = 7;
    d.self.nested_catalogs = 2;
    d.self.chunked_files   = 5;
    d.self.file_chunks     = 42;

    d.subtree.regular_files   = -22;
    d.subtree.symlinks        = 23;
    d.subtree.specials        = -2;
    d.subtree.directories     = 100;
    d.subtree.nested_catalogs = -1;
    d.subtree.chunked_files   = -5;
    d.subtree.file_chunks     = -32;
    return d;
  }

  Counters GetFilledCounters() const {
    Counters c;
    c.self.regular_files   = 152;
    c.self.symlinks        = 7;
    c.self.specials        = 20;
    c.self.directories     = 12;
    c.self.nested_catalogs = 4;
    c.self.chunked_files   = 100;
    c.self.file_chunks     = 1337;

    c.subtree.regular_files   = 82;
    c.subtree.symlinks        = 100;
    c.subtree.specials        = 5;
    c.subtree.directories     = 72;
    c.subtree.nested_catalogs = 2;
    c.subtree.chunked_files   = 23;
    c.subtree.file_chunks     = 75;
    return c;
  }
};


TEST_F(T_CatalogCounters, CounterInitialization) {
  Counters counters;
  EXPECT_EQ(Counters_t(0), counters.GetSelfEntries());
  EXPECT_EQ(Counters_t(0), counters.GetSubtreeEntries());
  EXPECT_EQ(Counters_t(0), counters.GetAllEntries());
}


TEST_F(T_CatalogCounters, DeltaInitialization) {
  DeltaCounters d_counters;;

  // just checking a random sample here...
  EXPECT_EQ(DeltaCounters_t(0), d_counters.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(0), d_counters.self.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(0), d_counters.subtree.chunked_files);
  EXPECT_EQ(DeltaCounters_t(0), d_counters.subtree.symlinks);
}


TEST_F(T_CatalogCounters, DeltaBasicIncrement) {
  DeltaCounters d_counters;

  DirectoryEntry regular_file  = DirectoryEntryTestFactory::RegularFile();
  DirectoryEntry directory     = DirectoryEntryTestFactory::Directory();
  DirectoryEntry symlink       = DirectoryEntryTestFactory::Symlink();
  DirectoryEntry chunked_file  = DirectoryEntryTestFactory::ChunkedFile();
  DirectoryEntry external_file = DirectoryEntryTestFactory::ExternalFile();
  DirectoryEntry special_file  = DirectoryEntryTestFactory::SpecialFile();

  d_counters.Increment(regular_file);
  d_counters.Increment(regular_file);
  d_counters.Increment(regular_file);

  d_counters.Increment(directory);
  d_counters.Increment(directory);

  d_counters.Increment(symlink);
  d_counters.Increment(symlink);
  d_counters.Increment(symlink);

  d_counters.Increment(chunked_file);
  d_counters.Increment(chunked_file);

  d_counters.Increment(external_file);

  d_counters.Increment(special_file);
  d_counters.Increment(special_file);

  EXPECT_EQ(DeltaCounters_t(6), d_counters.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(2), d_counters.self.directories);
  EXPECT_EQ(DeltaCounters_t(3), d_counters.self.symlinks);
  EXPECT_EQ(DeltaCounters_t(2), d_counters.self.chunked_files);
  EXPECT_EQ(DeltaCounters_t(1), d_counters.self.externals);
  EXPECT_EQ(DeltaCounters_t(2), d_counters.self.specials);
}


TEST_F(T_CatalogCounters, DeltaBasicDecrement) {
  DeltaCounters d_counters;
  d_counters.self.regular_files = 10;
  d_counters.self.directories   = 5;
  d_counters.self.symlinks      = 7;
  d_counters.self.specials      = 2;
  d_counters.self.chunked_files = 3;

  DirectoryEntry regular_file  = DirectoryEntryTestFactory::RegularFile();
  DirectoryEntry directory     = DirectoryEntryTestFactory::Directory();
  DirectoryEntry symlink       = DirectoryEntryTestFactory::Symlink();
  DirectoryEntry chunked_file  = DirectoryEntryTestFactory::ChunkedFile();
  DirectoryEntry external_file = DirectoryEntryTestFactory::ExternalFile();
  DirectoryEntry special_file  = DirectoryEntryTestFactory::SpecialFile();

  d_counters.Decrement(regular_file);
  d_counters.Decrement(regular_file);
  d_counters.Decrement(regular_file);

  d_counters.Decrement(directory);
  d_counters.Decrement(directory);

  d_counters.Decrement(symlink);
  d_counters.Decrement(symlink);
  d_counters.Decrement(symlink);

  d_counters.Decrement(chunked_file);
  d_counters.Decrement(chunked_file);
  d_counters.Decrement(chunked_file);
  d_counters.Decrement(chunked_file);
  d_counters.Decrement(chunked_file);
  d_counters.Decrement(chunked_file);

  d_counters.Decrement(external_file);

  d_counters.Decrement(special_file);

  EXPECT_EQ(DeltaCounters_t(0),  d_counters.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(3),  d_counters.self.directories);
  EXPECT_EQ(DeltaCounters_t(4),  d_counters.self.symlinks);
  EXPECT_EQ(DeltaCounters_t(1),  d_counters.self.specials);
  EXPECT_EQ(DeltaCounters_t(-3), d_counters.self.chunked_files);
  EXPECT_EQ(DeltaCounters_t(-1), d_counters.self.externals);
}


TEST_F(T_CatalogCounters, FieldsCombinations) {
  typedef TreeCountersBase<int>::Fields IntFields;
  IntFields a;
  IntFields b;

  a.regular_files   = 1;
  a.symlinks        = 9;
  a.specials        = 2;
  a.directories     = 3;
  a.nested_catalogs = 11;
  a.chunked_files   = 5;
  a.file_chunks     = 7;
  a.externals       = 9;

  b.regular_files   = 2;
  b.symlinks        = 10;
  b.specials        = 3;
  b.directories     = 4;
  b.nested_catalogs = 12;
  b.chunked_files   = 6;
  b.file_chunks     = 8;
  b.externals       = 2;

  IntFields c = a;
  IntFields d = b;

  a.Add(b);

  EXPECT_EQ(3,  a.regular_files);
  EXPECT_EQ(19, a.symlinks);
  EXPECT_EQ(5,  a.specials);
  EXPECT_EQ(7,  a.directories);
  EXPECT_EQ(23, a.nested_catalogs);
  EXPECT_EQ(11, a.chunked_files);
  EXPECT_EQ(15, a.file_chunks);
  EXPECT_EQ(11, a.externals);

  c.Add(a);
  c.Subtract(b);

  EXPECT_EQ(2,  c.regular_files);
  EXPECT_EQ(18, c.symlinks);
  EXPECT_EQ(4, c.specials);
  EXPECT_EQ(6,  c.directories);
  EXPECT_EQ(22, c.nested_catalogs);
  EXPECT_EQ(10, c.chunked_files);
  EXPECT_EQ(14, c.file_chunks);
  EXPECT_EQ(18, c.externals);

  d.Add(c);
  d.Add(c);
  d.Subtract(b);
  d.Subtract(d);

  EXPECT_EQ(0, d.regular_files);
  EXPECT_EQ(0, d.symlinks);
  EXPECT_EQ(0, d.specials);
  EXPECT_EQ(0, d.directories);
  EXPECT_EQ(0, d.nested_catalogs);
  EXPECT_EQ(0, d.chunked_files);
  EXPECT_EQ(0, d.file_chunks);
  EXPECT_EQ(0, d.externals);
}


TEST_F(T_CatalogCounters, DeltaPopulateToParent) {
  DeltaCounters d_parent = GetFilledDeltaCounters();

  DeltaCounters d_child;
  d_child.self.regular_files    = 10;
  d_child.self.directories      = 5;
  d_child.self.chunked_files    = 3;
  d_child.self.file_chunks      = 13;
  d_child.subtree.regular_files = 12;
  d_child.subtree.specials     = -1;
  d_child.subtree.chunked_files = -4;
  d_child.subtree.file_chunks   = 4;

  d_child.PopulateToParent(&d_parent);

  EXPECT_EQ(DeltaCounters_t(102), d_parent.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(10),  d_parent.self.symlinks);
  EXPECT_EQ(DeltaCounters_t(3),   d_parent.self.specials);
  EXPECT_EQ(DeltaCounters_t(7),   d_parent.self.directories);
  EXPECT_EQ(DeltaCounters_t(2),   d_parent.self.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(5),   d_parent.self.chunked_files);
  EXPECT_EQ(DeltaCounters_t(42),  d_parent.self.file_chunks);

  // self (10) + subtree (12)
  EXPECT_EQ(DeltaCounters_t(0),   d_parent.subtree.regular_files);
  EXPECT_EQ(DeltaCounters_t(23),  d_parent.subtree.symlinks);
  EXPECT_EQ(DeltaCounters_t(-3),  d_parent.subtree.specials);
  EXPECT_EQ(DeltaCounters_t(105), d_parent.subtree.directories);
  EXPECT_EQ(DeltaCounters_t(-1),  d_parent.subtree.nested_catalogs);
  // self (3)  + subtree (-4)
  EXPECT_EQ(DeltaCounters_t(-6),  d_parent.subtree.chunked_files);
  EXPECT_EQ(DeltaCounters_t(-15), d_parent.subtree.file_chunks);  // dito...
}


TEST_F(T_CatalogCounters, CountersBasic) {
  Counters counters = GetFilledCounters();

  EXPECT_EQ(Counters_t(191), counters.GetSelfEntries());
  EXPECT_EQ(Counters_t(259), counters.GetSubtreeEntries());
  EXPECT_EQ(Counters_t(450), counters.GetAllEntries());
}


TEST_F(T_CatalogCounters, ApplyDeltaToCounters) {
  DeltaCounters d = GetFilledDeltaCounters();
  Counters      c = GetFilledCounters();

  c.ApplyDelta(d);

  EXPECT_EQ(Counters_t(254),  c.self.regular_files);
  EXPECT_EQ(Counters_t(17),   c.self.symlinks);
  EXPECT_EQ(Counters_t(23),   c.self.specials);
  EXPECT_EQ(Counters_t(19),   c.self.directories);
  EXPECT_EQ(Counters_t(6),    c.self.nested_catalogs);
  EXPECT_EQ(Counters_t(105),  c.self.chunked_files);
  EXPECT_EQ(Counters_t(1379), c.self.file_chunks);

  EXPECT_EQ(Counters_t(60),   c.subtree.regular_files);
  EXPECT_EQ(Counters_t(123),  c.subtree.symlinks);
  EXPECT_EQ(Counters_t(3),    c.subtree.specials);
  EXPECT_EQ(Counters_t(172),  c.subtree.directories);
  EXPECT_EQ(Counters_t(1),    c.subtree.nested_catalogs);
  EXPECT_EQ(Counters_t(18),   c.subtree.chunked_files);
  EXPECT_EQ(Counters_t(43),   c.subtree.file_chunks);
}


TEST_F(T_CatalogCounters, MergeIntoParent) {
  Counters      c_child  = GetFilledCounters();
  DeltaCounters d_parent = GetFilledDeltaCounters();

  c_child.MergeIntoParent(&d_parent);

  EXPECT_EQ(DeltaCounters_t(254),   d_parent.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(17),    d_parent.self.symlinks);
  EXPECT_EQ(DeltaCounters_t(23),    d_parent.self.specials);
  EXPECT_EQ(DeltaCounters_t(19),    d_parent.self.directories);
  EXPECT_EQ(DeltaCounters_t(6),     d_parent.self.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(105),   d_parent.self.chunked_files);
  EXPECT_EQ(DeltaCounters_t(1379),  d_parent.self.file_chunks);

  EXPECT_EQ(DeltaCounters_t(-174),  d_parent.subtree.regular_files);
  EXPECT_EQ(DeltaCounters_t(16),    d_parent.subtree.symlinks);
  EXPECT_EQ(DeltaCounters_t(-22),   d_parent.subtree.specials);
  EXPECT_EQ(DeltaCounters_t(88),    d_parent.subtree.directories);
  EXPECT_EQ(DeltaCounters_t(-5),    d_parent.subtree.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(-105),  d_parent.subtree.chunked_files);
  EXPECT_EQ(DeltaCounters_t(-1369), d_parent.subtree.file_chunks);
}


TEST_F(T_CatalogCounters, AddAsSubtree) {
  Counters      c_child  = GetFilledCounters();
  DeltaCounters d_parent = GetFilledDeltaCounters();

  c_child.AddAsSubtree(&d_parent);

  EXPECT_EQ(DeltaCounters_t(102),  d_parent.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(10),   d_parent.self.symlinks);
  EXPECT_EQ(DeltaCounters_t(3),    d_parent.self.specials);
  EXPECT_EQ(DeltaCounters_t(7),    d_parent.self.directories);
  EXPECT_EQ(DeltaCounters_t(2),    d_parent.self.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(5),    d_parent.self.chunked_files);
  EXPECT_EQ(DeltaCounters_t(42),   d_parent.self.file_chunks);

  EXPECT_EQ(DeltaCounters_t(212),  d_parent.subtree.regular_files);
  EXPECT_EQ(DeltaCounters_t(130),  d_parent.subtree.symlinks);
  EXPECT_EQ(DeltaCounters_t(23),   d_parent.subtree.specials);
  EXPECT_EQ(DeltaCounters_t(184),  d_parent.subtree.directories);
  EXPECT_EQ(DeltaCounters_t(5),    d_parent.subtree.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(118),  d_parent.subtree.chunked_files);
  EXPECT_EQ(DeltaCounters_t(1380), d_parent.subtree.file_chunks);
}


TEST_F(T_CatalogCounters, Diff) {
  Counters counters = GetFilledCounters();
  Counters empty;
  DeltaCounters delta = Counters::Diff(empty, counters);
  EXPECT_EQ(DeltaCounters_t(152),  delta.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(7),    delta.self.symlinks);
  EXPECT_EQ(DeltaCounters_t(20),   delta.self.specials);
  EXPECT_EQ(DeltaCounters_t(12),   delta.self.directories);
  EXPECT_EQ(DeltaCounters_t(4),    delta.self.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(100),  delta.self.chunked_files);
  EXPECT_EQ(DeltaCounters_t(1337), delta.self.file_chunks);
  EXPECT_EQ(DeltaCounters_t(82),   delta.subtree.regular_files);
  EXPECT_EQ(DeltaCounters_t(100),  delta.subtree.symlinks);
  EXPECT_EQ(DeltaCounters_t(5),    delta.subtree.specials);
  EXPECT_EQ(DeltaCounters_t(72),   delta.subtree.directories);
  EXPECT_EQ(DeltaCounters_t(2),    delta.subtree.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(23),   delta.subtree.chunked_files);
  EXPECT_EQ(DeltaCounters_t(75),   delta.subtree.file_chunks);

  delta = Counters::Diff(counters, empty);
  EXPECT_EQ(DeltaCounters_t(-152),  delta.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(-7),    delta.self.symlinks);
  EXPECT_EQ(DeltaCounters_t(-20),   delta.self.specials);
  EXPECT_EQ(DeltaCounters_t(-12),   delta.self.directories);
  EXPECT_EQ(DeltaCounters_t(-4),    delta.self.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(-100),  delta.self.chunked_files);
  EXPECT_EQ(DeltaCounters_t(-1337), delta.self.file_chunks);
  EXPECT_EQ(DeltaCounters_t(-82),   delta.subtree.regular_files);
  EXPECT_EQ(DeltaCounters_t(-100),  delta.subtree.symlinks);
  EXPECT_EQ(DeltaCounters_t(-5),    delta.subtree.specials);
  EXPECT_EQ(DeltaCounters_t(-72),   delta.subtree.directories);
  EXPECT_EQ(DeltaCounters_t(-2),    delta.subtree.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(-23),   delta.subtree.chunked_files);
  EXPECT_EQ(DeltaCounters_t(-75),   delta.subtree.file_chunks);

  delta = Counters::Diff(counters, counters);
  EXPECT_EQ(DeltaCounters_t(0), delta.self.regular_files);
  EXPECT_EQ(DeltaCounters_t(0), delta.self.symlinks);
  EXPECT_EQ(DeltaCounters_t(0), delta.self.specials);
  EXPECT_EQ(DeltaCounters_t(0), delta.self.directories);
  EXPECT_EQ(DeltaCounters_t(0), delta.self.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(0), delta.self.chunked_files);
  EXPECT_EQ(DeltaCounters_t(0), delta.self.file_chunks);
  EXPECT_EQ(DeltaCounters_t(0), delta.subtree.regular_files);
  EXPECT_EQ(DeltaCounters_t(0), delta.subtree.symlinks);
  EXPECT_EQ(DeltaCounters_t(0), delta.subtree.directories);
  EXPECT_EQ(DeltaCounters_t(0), delta.subtree.nested_catalogs);
  EXPECT_EQ(DeltaCounters_t(0), delta.subtree.chunked_files);
  EXPECT_EQ(DeltaCounters_t(0), delta.subtree.file_chunks);
}


TEST_F(T_CatalogCounters, FieldsMap) {
  DeltaCounters d_counters;
  DeltaCounters d_parent;

  DirectoryEntry regular_file  = DirectoryEntryTestFactory::RegularFile();
  DirectoryEntry directory     = DirectoryEntryTestFactory::Directory();
  DirectoryEntry symlink       = DirectoryEntryTestFactory::Symlink();
  DirectoryEntry special       = DirectoryEntryTestFactory::SpecialFile();
  DirectoryEntry chunked_file  = DirectoryEntryTestFactory::ChunkedFile();
  DirectoryEntry external_file = DirectoryEntryTestFactory::ExternalFile();

  DeltaCounters::FieldsMap map;

  d_counters.Increment(regular_file);
  d_counters.Increment(regular_file);
  d_counters.Increment(regular_file);

  map = d_counters.GetFieldsMap();
  EXPECT_EQ(DeltaCounters_t(3), *map["self_regular"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_regular"]);

  d_counters.Increment(directory);
  d_counters.Increment(directory);

  map = d_counters.GetFieldsMap();
  EXPECT_EQ(DeltaCounters_t(3), *map["self_regular"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_regular"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["self_dir"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_dir"]);

  d_counters.Increment(symlink);
  d_counters.Increment(symlink);
  d_counters.Increment(symlink);

  map = d_counters.GetFieldsMap();
  EXPECT_EQ(DeltaCounters_t(3), *map["self_regular"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_regular"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["self_dir"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_dir"]);
  EXPECT_EQ(DeltaCounters_t(3), *map["self_symlink"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_symlink"]);

  d_counters.Increment(special);

  map = d_counters.GetFieldsMap();
  EXPECT_EQ(DeltaCounters_t(3), *map["self_regular"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_regular"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["self_dir"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_dir"]);
  EXPECT_EQ(DeltaCounters_t(3), *map["self_symlink"]);
  EXPECT_EQ(DeltaCounters_t(1), *map["self_special"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_symlink"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_special"]);

  d_counters.Increment(chunked_file);
  d_counters.Increment(chunked_file);

  map = d_counters.GetFieldsMap();
  EXPECT_EQ(DeltaCounters_t(5), *map["self_regular"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_regular"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["self_dir"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_dir"]);
  EXPECT_EQ(DeltaCounters_t(3), *map["self_symlink"]);
  EXPECT_EQ(DeltaCounters_t(1), *map["self_special"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_symlink"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["self_chunked"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_chunked"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_special"]);

  d_counters.Increment(external_file);

  map = d_counters.GetFieldsMap();
  EXPECT_EQ(DeltaCounters_t(6), *map["self_regular"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_regular"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["self_dir"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_dir"]);
  EXPECT_EQ(DeltaCounters_t(3), *map["self_symlink"]);
  EXPECT_EQ(DeltaCounters_t(1), *map["self_special"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_symlink"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["self_chunked"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_chunked"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_special"]);
  EXPECT_EQ(DeltaCounters_t(1), *map["self_external"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["subtree_external"]);

  d_counters.PopulateToParent(&d_parent);

  map = d_parent.GetFieldsMap();
  EXPECT_EQ(DeltaCounters_t(0), *map["self_regular"]);
  EXPECT_EQ(DeltaCounters_t(6), *map["subtree_regular"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["self_dir"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["subtree_dir"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["self_symlink"]);
  EXPECT_EQ(DeltaCounters_t(3), *map["subtree_symlink"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["self_chunked"]);
  EXPECT_EQ(DeltaCounters_t(2), *map["subtree_chunked"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["self_external"]);
  EXPECT_EQ(DeltaCounters_t(1), *map["subtree_external"]);
  EXPECT_EQ(DeltaCounters_t(1), *map["subtree_special"]);
  EXPECT_EQ(DeltaCounters_t(0), *map["self_special"]);
}

TEST_F(T_CatalogCounters, CsvMap) {
  Counters c = GetFilledCounters();
  std::string output_actual = c.GetCsvMap();
  std::string output_expected =
    "chunked," + StringifyInt(c.self.chunked_files + c.subtree.chunked_files) +
    "\nchunked_size," +
    StringifyInt(c.self.chunked_file_size + c.subtree.chunked_file_size) +
    "\nchunks," + StringifyInt(c.self.file_chunks + c.subtree.file_chunks) +
    "\ndir," + StringifyInt(c.self.directories + c.subtree.directories) +
    "\nexternal," + StringifyInt(c.self.externals + c.subtree.externals) +
    "\nexternal_file_size," +
    StringifyInt(c.self.external_file_size + c.subtree.external_file_size) +
    "\nfile_size," + StringifyInt(c.self.file_size + c.subtree.file_size) +
    "\nnested," +
    StringifyInt(c.self.nested_catalogs + c.subtree.nested_catalogs) +
    "\nregular," +
    StringifyInt(c.self.regular_files + c.subtree.regular_files) +
    "\nspecial," + StringifyInt(c.self.specials + c.subtree.specials) +
    "\nsymlink," + StringifyInt(c.self.symlinks + c.subtree.symlinks) +
    "\nxattr," + StringifyInt(c.self.xattrs + c.subtree.xattrs) + "\n";
  EXPECT_EQ(output_expected, output_actual);
}

}  // namespace catalog
