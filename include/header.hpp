// Copyright [2020] <Alyona Dorodnyaya>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <iostream>
#include <map>

bool financial_info(const std::string& name) {
  if (name.size() != 25 || name.substr(0, 8) != "balance_" ||
      name.at(16) != '_')
    return false;
  // если номер счёт и даты не из цифр
  for (size_t i = 8; i < name.size(); i++)
    if (i != 16)
      if (!((int)name.at(i) >= 48 && (int)name.at(i) <= 57)) return false;
  // проверим формат даты
  if (stoi(name.substr(21, 2)) > 12) return false;
  if (stoi(name.substr(23, 2)) > 31) return false;
  return true;
}

// колличество документов по счёту, дата последнего
struct Fin_file {
  size_t count = 0;
  std::string lastdate;
  bool operator==(const Fin_file& r) const {
    return (count == r.count && lastdate == r.lastdate);
  }
};

// добавление элемента в map - если такого счёта нет или если дата новее
void data(std::map<std::string, Fin_file>& summ_brok, const std::string& name) {
  std::string number = name.substr(8, 8);
  int date = stoi(name.substr(17, 8));

  if (summ_brok.find(number) == summ_brok.end()) {
    summ_brok[number].lastdate = name.substr(17, 8);
  } else {
    int date_ = stoi(summ_brok[number].lastdate.substr(0, 8));
    if (date_ < date) summ_brok[number].lastdate = name.substr(17, 8);
  }
  summ_brok[number].count++;
}

#endif  // INCLUDE_HEADER_HPP_
