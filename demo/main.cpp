// Copyright [2020] <Alyona Dorodnyaya>

#include <boost/filesystem.hpp>
#include <iostream>
#include <map>

namespace fs = boost::filesystem;

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
  std::string date;
};

// добавление элемента в map - если такого счёта нет или если дата новее
void data(std::map<std::string, Fin_file>& summ_brok, const std::string& name) {
  std::string number = name.substr(8, 8);
  int year = stoi(name.substr(17, 4));
  int month = stoi(name.substr(21, 2));
  int day = stoi(name.substr(23, 2));

  if (summ_brok.find(number) == summ_brok.end()) {
    summ_brok[number].count++;
    summ_brok[number].date = name.substr(17, 8);
  } else {
    int year_ = stoi(summ_brok[number].date.substr(0, 4));
    int month_ = stoi(summ_brok[number].date.substr(4, 2));
    int day_ = stoi(summ_brok[number].date.substr(6, 2));

    summ_brok[number].count++;
    if (year_ < year || (year_ == year && month_ < month) ||
        (year_ == year && month_ == month && day_ < day))
      summ_brok[number].date = name.substr(17, 8);
  }
}

int main(int argc, char* argv[]) {
  // fs::path path_to_ftp = (argc > 1) ? argv[1] : "../test_file"; для
  // тестирования

  fs::path path_to_ftp = (argc > 1) ? argv[1] : fs::current_path();

  if (!exists(path_to_ftp)) throw std::invalid_argument("Don't found FTP-file");

  // map: ключ - имя брокера, значение - счета, колличество документов по ним
  // и дата последнего
  std::map<std::string, std::map<std::string, Fin_file>> summ;
  for (const fs::directory_entry& broker :
       fs::directory_iterator{path_to_ftp}) {
    // неверный формат файла - не состоит из дирикторий, ненайдена
    // директория
    if (!exists(broker.path()))
      throw std::invalid_argument("Invalid FTP-file: don't found\"" +
                                  broker.path().string() + "\"");
    if (!is_directory(broker.path()))
      throw std::invalid_argument("Invalid FTP-file: \"" +
                                  broker.path().string() +
                                  "\" isn't directory");

    // проходимся по дириктории
    // map: ключ - счёт, значение - колличество документов по ним
    // и дата последнего
    std::map<std::string, Fin_file> summ_brok;
    for (const fs::directory_entry& x : fs::directory_iterator{broker.path()}) {
      auto file = x.path();

      // неверный формат файла - ненайден файл, это директория
      if (!exists(file))
        throw std::invalid_argument("Invalid FTP-file: don't found\"" +
                                    file.string() + "\"");

      if (is_directory(file))
        throw std::invalid_argument("Invalid FTP-file: \"" + file.string() +
                                    "\" isn't file");

      // если символьная ссылка - проверяем еще раз на существование файла
      if (is_symlink(file)) {
        file = read_symlink(file);
        if (!exists(file))
          throw std::invalid_argument("Invalid FTP-file: don't found\"" +
                                      file.string() + "\"");
      }

      // если это файл, с разрешением .txt, удовлетворяющий условию
      if (is_regular_file(file) && file.extension().string() == ".txt" &&
          file.stem().extension().string() != ".old" &&
          financial_info(file.stem().string())) {
        // для первого задания
        std::cout << broker.path().stem().string() << ' '
                  << file.filename().string() << std::endl;

        data(summ_brok, file.stem().string());
      }
    }
    summ[broker.path().stem().string()] = summ_brok;
  }
  std::cout << '\n';

  for (const auto& broker : summ)
    for (const auto& number : broker.second) {
      std::cout << "broker:" << broker.first << " account:" << number.first;
      std::cout << " files: " << number.second.count
                << " lastdate:" << number.second.date << std::endl;
    }
}
