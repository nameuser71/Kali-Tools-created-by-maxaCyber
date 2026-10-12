#include <iostream>
#include <string>
#include <vector>
#include <omp.h>
#include <chrono>
#include <cstring>

const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int CHARSET_SIZE = CHARSET.size();

// Help menyusi uchun funksiya
void print_help(const std::string& lang) {
    if (lang == "uz") {
        std::cout << "========================================================\n";
        std::cout << "               Dasturdan foydalanish yo'riqnomasi        \n";
        std::cout << "========================================================\n";
        std::cout << "Buyruqlar:\n";
        std::cout << "  ./full_brute              - Dasturni standart rejimda ishga tushirish\n";
        std::cout << "  ./full_brute -h uz        - Yordam menyusini o'zbek tilida ko'rsatish\n";
        std::cout << "  ./full_brute -h en        - Yordam menyusini ingliz tilida ko'rsatish\n";
        std::cout << "  ./full_brute -h ru        - Yordam menyusini rus tilida ko'rsatish\n\n";
        std::cout << "Tavsif:\n";
        std::cout << "  Ushbu dastur OpenMP texnologiyasi yordamida barcha CPU yadrolaridan\n";
        std::cout << "  foydalanib yuqori tezlikda incremental qidiruvni amalga oshiradi.\n";
        std::cout << "========================================================\n";
    } 
    else if (lang == "ru") {
        std::cout << "========================================================\n";
        std::cout << "             Руководство по использованию                \n";
        std::cout << "========================================================\n";
        std::cout << "Команды:\n";
        std::cout << "  ./full_brute              - Запуск программы в стандартном режиме\n";
        std::cout << "  ./full_brute -h uz        - Показать справку на узбекском языке\n";
        std::cout << "  ./full_brute -h en        - Показать справку на английском языке\n";
        std::cout << "  ./full_brute -h ru        - Показать справку на русском языке\n\n";
        std::cout << "Описание:\n";
        std::cout << "  Программа использует OpenMP для параллельных вычислений на всех\n";
        std::cout << "  ядрах процессора с максимальной производительностью.\n";
        std::cout << "========================================================\n";
    } 
    else { // Standard: English
        std::cout << "========================================================\n";
        std::cout << "                   Usage Instructions                   \n";
        std::cout << "========================================================\n";
        std::cout << "Commands:\n";
        std::cout << "  ./full_brute              - Run the program in default mode\n";
        std::cout << "  ./full_brute -h uz        - Show help menu in Uzbek\n";
        std::cout << "  ./full_brute -h en        - Show help menu in English\n";
        std::cout << "  ./full_brute -h ru        - Show help menu in Russian\n\n";
        std::cout << "Description:\n";
        std::cout << "  High-performance incremental brute-force engine utilizing\n";
        std::cout << "  OpenMP multi-threading across all available CPU cores.\n";
        std::cout << "========================================================\n";
    }
}

long long power(int base, int exp) {
    long long res = 1;
    for (int i = 0; i < exp; ++i) res *= base;
    return res;
}

bool crack_fixed_length(int length, const std::string& target, std::string& found_password) {
    long long total_combinations = power(CHARSET_SIZE, length);
    bool found = false;

    #pragma omp parallel for schedule(dynamic, 1000) shared(found, found_password)
    for (long long i = 0; i < total_combinations; ++i) {
        if (found) continue;

        std::string candidate(length, ' ');
        long long temp = i;
        for (int j = length - 1; j >= 0; --j) {
            candidate[j] = CHARSET[temp % CHARSET_SIZE];
            temp /= CHARSET_SIZE;
        }

        if (candidate == target) {
            #pragma omp critical
            {
                found_password = candidate;
                found = true;
            }
        }
    }

    return found;
}

int main(int argc, char* argv[]) {
    // Buyruqlar satri argumentlarini tahlil qilish (-h uz, -h en, -h ru)
    if (argc > 1) {
        std::string arg1 = argv[1];
        if (arg1 == "-h" || arg1 == "--help") {
            std::string lang = "en"; // Standart til
            if (argc > 2) {
                lang = argv[2];
            }
            print_help(lang);
            return 0;
        }
    }

    std::string target = "ab12"; 
    int min_length = 1;
    int max_length = 6; 

    std::cout << "[*] Yuqori unumdorlikdagi Engine boshlandi...\n";
    std::cout << "[*] CPU yadrolari soni: " << omp_get_max_threads() << "\n";
    std::cout << "[*] Qidirilayotgan qiymat: " << target << "\n\n";

    auto start_time = std::chrono::high_resolution_clock::now();
    std::string found_password = "";
    bool success = false;

    for (int len = min_length; len <= max_length; ++len) {
        std::cout << "[*] Tekshirilmoqda: " << len << " xonali kombinatsiyalar...\n";
        if (crack_fixed_length(len, target, found_password)) {
            success = true;
            break;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    if (success) {
        std::cout << "\n[+] MUVAFFAQIYAT! Natija: " << found_password << "\n";
    } else {
        std::cout << "\n[-] Belgilangan oraliqda topilmadi.\n";
    }
    
    std::cout << "[*] Sarflangan vaqt: " << elapsed.count() << " soniya\n";

    return 0;
}
