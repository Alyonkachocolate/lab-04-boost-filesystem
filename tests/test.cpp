// Copyright [2020] <Alyona Dorodnyaya>

#include <gtest/gtest.h>

#include <header.hpp>

TEST(Boost_filesystem, financial_info) {
  EXPECT_TRUE(financial_info("balance_00000001_20180102"));
  // неверный формат (размер != 25)
  EXPECT_FALSE(financial_info("information"));
  EXPECT_FALSE(financial_info("balance_"));
  EXPECT_FALSE(financial_info("00000001_20180102"));
  EXPECT_FALSE(financial_info("balance_1_20180102"));
  // неверный формат счёта
  EXPECT_FALSE(financial_info("balance_000а0001_20180102"));
  // неверный формат даты
  EXPECT_FALSE(financial_info("balance_01056804_201u0102"));
  // неверный формат месяца
  EXPECT_FALSE(financial_info("balance_01056804_20181502"));
  // неверный формат дня
  EXPECT_FALSE(financial_info("balance_01056804_20180134"));
}

TEST(Boost_filesystem, data) {
  std::map<std::string, Fin_file> summ_brok;
  data(summ_brok, "balance_01056804_20201102");
  EXPECT_EQ(summ_brok["01056804"], (Fin_file{1, "20201102"}));
  // тот же счёт, дата раньше (20201101 < 20201102)
  data(summ_brok, "balance_01056804_20201101");
  EXPECT_EQ(summ_brok["01056804"], (Fin_file{2, "20201102"}));
  // тот же счёт, дата позднее (20201111 > 20201102)
  data(summ_brok, "balance_01056804_20201111");
  EXPECT_EQ(summ_brok["01056804"], (Fin_file{3, "20201111"}));
  // новый счёт
  data(summ_brok, "balance_11111111_20200214");
  EXPECT_EQ(summ_brok["01056804"], (Fin_file{3, "20201111"}));
  EXPECT_EQ(summ_brok["11111111"], (Fin_file{1, "20200214"}));
}
