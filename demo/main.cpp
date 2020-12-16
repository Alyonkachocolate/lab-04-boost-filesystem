// Copyright [2020] <Alyona Dorodnyaya>

#include <boost/filesystem.hpp>
#include <header.hpp>
#include <iostream>
#include <map>

namespace fs = boost::filesystem;

int main(int argc, char* argv[]) {
//     для тестирования
//    fs::path path_to_ftp = (argc > 1) ? argv[1] : "../test_file";

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
                << " lastdate:" << number.second.lastdate << std::endl;
    }
}
