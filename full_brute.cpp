#include <iostream>
#include <string>
#include <vector>
#include <omp.h>
#include <chrono>

// Kerakli belgilar jamlanmasi (Charsetni xohishga ko'ra o'zgartirishingiz mumkin)
const std::string CHARSET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int CHARSET_SIZE = CHARSET.size();

// Berilgan uzunlikdagi jami kombinatsiyalar sonini hisoblash (masalan: CHARSET_SIZE^length)
long long power(int base, int exp) {
    long long res = 1;
    for (int i = 0; i < exp; ++i) {
        res *= base;
    }
    return res;
}

// Bitta uzunlikdagi qismni parallel ravishda tekshirish
bool crack_fixed_length(int length, const std::string& target, std::string& found_password) {
    long long total_combinations = power(CHARSET_SIZE, length);
    bool found = false;

    #pragma omp parallel for schedule(dynamic, 1000) shared(found, found_password)
    for (long long i = 0; i < total_combinations; ++i) {
        if (found) continue; // Agar boshqa yadro topgan bo'lsa, to'xtash

        std::string candidate(length, ' ');
        long long temp = i;
        for (int j = length - 1; j >= 0; --j) {
            candidate[j] = CHARSET[temp % CHARSET_SIZE];
            temp /= CHARSET_SIZE;
        }

        // --- BU YERDA TEKSHIRUV AMALGA OSHIRILADI ---
        // Agar aniq parol tekshirilsa:
        if (candidate == target) {
            #pragma omp critical
            {
                found_password = candidate;
                found = true;
            }
        }
        
        /* 
           Agar Hash tekshirilsa (masalan, SHA256 yoki MD5):
           if (sha256(candidate) == target_hash) { ... }
        */
    }

    return found;
}

int main() {
    // Qidirilayotgan maqsad (Parol yoki Hash)
    std::string target = "ab12"; 
    int min_length = 1;
    int max_length = 6; // Eslatma: Uzunlik oshgani sayin kombinatsiyalar keskin ko'payadi!

    std::cout << "[*] Yuqori unumdorlikdagi Incremental Brute-Force boshlandi...\n";
    std::cout << "[*] CPU yadrolari soni: " << omp_get_max_threads() << "\n";
    std::cout << "[*] Qidirilayotgan qiymat: " << target << "\n\n";

    auto start_time = std::chrono::high_resolution_clock::now();
    std::string found_password = "";
    bool success = false;

    // Incremental qism: uzunlikni 1 dan boshlab bosqichma-bosqich oshirib boramiz
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
        std::cout << "\n[+] MUVAFFAQIYAT! Parol topildi: " << found_password << "\n";
    } else {
        std::cout << "\n[-] Belgilangan uzunlik oralig'ida topilmadi.\n";
    }
    
    std::cout << "[*] Sarflangan vaqt: " << elapsed.count() << " soniya\n";

    return 0;
}
