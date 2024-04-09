#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unicode/unistr.h>
using namespace std;
using std::cout;
using std::cin;
using std::string;
using json = nlohmann::json;
using ustr = icu::UnicodeString;

class Product { // основной класс товара
private:
    string name;
    string category;
    float price;
    float weight;

public:
    Product(const string& name, const string& category, float price, float weight): // конструктор класса
    name(name), category(category), price(price), weight(weight) {}

    void set_name(const string& new_name) {  //сеттер для имени
        name = new_name;
    }

    void set_category(const  string& new_category) {  //сеттер для категории
        category = new_category;
    }

    void set_price(float new_price) {  //сеттер для цены
        price = new_price;
    }

    void set_weight(float new_weight) {  //сеттер для веса
        weight = new_weight;
    }

    // геттеры для всех полей:
    string get_name() const { return name; }
    string get_category() const { return category; }
    float get_price() const { return price; }
    float get_weight() const { return weight; }
};

class Buy : public Product {  // класс покупки, дочерний для класса товара и роительский для класса проверки
private:
    int quantity;
    float totalCost;

public:
    Buy(const string& name, const string& category, float price, float weight, int quantity): 
    Product(name, category, price, weight), quantity(quantity), totalCost(price * quantity) {}

    void set_quantity(int new_quantity) {
        quantity = new_quantity;
        totalCost = get_price() * new_quantity; // Обновляем общую стоимость при изменении количества
    }

    int get_quantity() const { return quantity; }
    float get_total_cost() const { return totalCost; }
};

class Check : public Buy {  // класс чека, дочерний для класса покупки
public:
    Check(const string& name, const string& category, float price, float weight, int quantity=0): 
    Buy(name, category, price, weight, quantity) {}

    void print_check() const {
        cout << "Товар: " << get_name() << "\n"
        << "Категория: " << get_category() << "\n"
        << "Вес: " << get_weight() << "\n"
        << "Цена за штуку: " << get_price() << "\n"
        << "Количество: " << get_quantity() << "\n"
        << "Итого к оплате: " << get_total_cost()<<"\n"<<endl;
    }
};

bool data_valid(const string& name, float price, float weight, const string& category="___"){
    ustr uc_name(name.c_str(), "UTF-8");
    ustr uc_cat(category.c_str(), "UTF-8");
    if(uc_name.isEmpty() || uc_name.length()>30){
        cerr << "Некорректное название товара! (Название должно быть непустым и не более 30 символов)" << endl;
        return false;
    }else if(price<0){
        cerr << "Некорректная цена! (Цена должна быть больше нуля)" << endl;
        return false;
    }else if(weight<0){
        cerr << "Некорректный вес! (Вес должен быть больше нуля)" << endl;
        return false;
    }else if(uc_cat.isEmpty() || uc_cat.length()>30){
        cerr << "Некорректное название категории! (Категория должна быть непустой и не более 30 символов)" << endl;
        return false;
    }else{
        return true;
    }
}

void print_product(const Check& product) {
    cout << "Название: " << product.get_name()
    << ", Категория: " << product.get_category()
    << ", Цена: " << product.get_price()
    << ", Вес: " << product.get_weight()<<endl;
}

void edit_file(vector<Check>& products){
    string name;
    cout << "Введите название товара который требуется изменить: ";
    cin >> name;
    cin.ignore();

    for (auto& product : products) {  // ищем в списке товар с таким названием
        if (product.get_name() == name) {
            float new_price, new_weight;
            string new_name;

            cout << "Введите новое название: ";
            getline(cin, new_name);
            cout << "Введите новую цену: ";
            cin >> new_price;
            cout << "Введите новый вес: ";
            cin >> new_weight;
            cin.ignore();

            if(data_valid(new_name, new_price, new_weight)){
                product.set_name(new_name);
                product.set_price(new_price);
                product.set_weight(new_weight);
                cout << "Товар успешно обновлен:\n";
                print_product(product);
                cout << "\n" << endl;
                return;
            } else{
                cerr << "Неверные данные для изменения.\n";
                return;
            }
        }
    }
    cerr << "Товар с таким названием не найден.\n" << endl;
    return;   
}

void purchase(vector<Check>& products) {
    string name;
    int quantity;
    float totalCost;

    cout << "Введите название товара для покупки: ";
    getline(cin, name);
    cout << "Сколько товара в штуках требуется? \n";
    cin >> quantity;
    cin.ignore();

    for (auto& product : products) {
        if (product.get_name() == name) {
            product.set_quantity(quantity);
            totalCost = product.get_total_cost();
            product.print_check();
            return;
        }
    }
    cout << "Товар с таким названием не найден.\n";
}       

void save2json(const vector<Check>& products, const string& filename) { // сохранение вектора продуктов в json
    json tmp;
    json jsonProducts;

    for (const auto& product : products) {
        tmp.push_back({
            {"name", product.get_name()},
            {"category", product.get_category()},
            {"price", product.get_price()},
            {"weight", product.get_weight()}
        });
    }
    jsonProducts["products"] = tmp;

    // Открываем файловый поток для записи
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        throw runtime_error("Не удалось открыть файл для записи!");
    }
    outFile << jsonProducts.dump(2);
    outFile.close();
}

template<typename T, typename Compare>  // шаблон для работы с разными типами данными
int partition(vector<T>& vec, int start, int end, Compare comp) { // функция разделения для быстрой сортировки
    T pivot = vec[end]; // опорный элемент (последний)
    int i = (start - 1); // индекс для поиска правильного места для опорного элемента (индекс последнего эл. меньшего опорного)

    for (int j = start; j <= end - 1; j++) {
        // Если текущий элемент меньше опорного
        if (comp(vec[j], pivot)) {
            i++;
            swap(vec[i], vec[j]);
        }
    }
    swap(vec[i + 1], vec[end]);  // свапаем опорный элемент с последним меньшим опорного
    return (i + 1);  // получаем индекс опорного элемемнта, который находится на своем правильном месте
}

template<typename T, typename Compare>
void quickSort(vector<T>& vec, int start, int end, Compare comp) {  // сама функция быстрой сортировки
    if (start < end) {
        // pi - индекс разбиения, vec[pi] сейчас на правильном месте
        int pi = partition(vec, start, end, comp);

        // Рекурсивно сортируем элементы до разбиения и после разбиения
        quickSort(vec, start, pi - 1, comp);
        quickSort(vec, pi + 1, end, comp);
    }
}

template<typename T, typename Compare>
void insertionSort(vector<T>& vec, Compare comp) {  // сортировка вставками
    for (size_t i = 1; i < vec.size(); i++) {
        T key = vec[i];
        int j = i - 1;

        // Перемещаем элементы до vec[i-1], которые больше key, на одну позицию вперед от текущей
        while (j >= 0 && comp(key, vec[j])) { // пока не начало массива и пока значение vec[i] меньше vec[j]
            vec[j + 1] = vec[j];
            j--;
        }
        vec[j + 1] = key;  // вставляем значение перед первым меньшим элементом 
    }
}

void search_w_name_categ(const vector<Check>& products, const string& name, const string& category) {
    bool found = false;
    for (const auto& product : products) {
        if (product.get_name() == name && product.get_category() == category) {
            cout << "Товар найден: " << endl;
            print_product(product);
            found = true;
        }
    }
    if (!found) {
        cout << "Товары с названием \"" << name << "\" и категорией \"" << category << "\" не найдены." << endl;
    }
}

void equal_prices(const unordered_map<float, vector<Check>>& Price2Products, float price) {
    auto it = Price2Products.find(price);
    if (it != Price2Products.end()) {
        cout << "Товары с ценой " << price << ":" << endl;
        for (const auto& product : it->second) {
            print_product(product);
        }
    } else {
        cout << "Товары с ценой " << price << " не найдены." << endl;
    }
}

int main(){
    cout << "Загружаю данные из файла \"products.json\"..."<<endl;
    string filename = "products.json";
    ifstream file(filename);
    if (!file.is_open()) {
        cout << ("Не удалось считать файл!")<<endl;
        return 1;
    }
    // Считываем данные из файла
    json jsonData;
    file >> jsonData;
    file.close();

    vector<Check> products;
    for (const auto& item : jsonData["products"]) {  //предполагаем, что в файле массив товаров объявлен после ключа "products"
        // Получаем данные для каждого товара
        string name = item["name"];
        float price = item["price"];
        float weight = item["weight"];
        string category = item["category"];

        if(data_valid(name, price, weight, category)){
            // Создание объектов Product и добавление их в вектор
            products.emplace_back(name, category, price, weight);
        }else{
            cout<<"Якобы некорректный элемент: "<<item<<endl;
            continue;  // пропускаем некорректные данные, но можем останавливать процесс полностью
        }
    }
    cout << "Данные загружены\n" << endl;
    unordered_map<float, vector<Check>> Price2Products;
    for (const auto& product : products) {
        Price2Products[product.get_price()].push_back(product);
    }

    // Предоставление пользователю выбора действия
    int action = -1;
    char desk = '+';
    string name, category;
    float price;
    while (action != 0) {
        cout << "Выберите действие с данными:\n"
        << "1 - Обновить данные товара\n"
        << "2 - Покупка товара\n"
        << "3 - Отсортировать товары по цене\n"
        << "4 - Отсортировать товары по названию\n"
        << "5 - Отсортировать товары по категории\n"
        << "6 - Найти товар по имени и категории\n"
        << "7 - Найти товары по цене (быстро)\n"
        << "0 - Выйти\n"
        << "Ввод: ";
        cin >> action;
        cin.ignore();

        switch (action) {
            case 0:
                cout << "Выход из программы..." << endl;
                return 0; // Выход из программы
            case 1:
                edit_file(products);  // редактируем данные
                save2json(products, filename); // Сохраняем обновления в json
                break;
            case 2:
                purchase(products);
                break;
            case 3:  // здесь и в случае 5 будем сортировать быстрой сортировкой
                quickSort(products, 0, products.size() - 1,
                [](const Check& a, const Check& b) { return a.get_price() < b.get_price(); });

                cout << "Сортировка по возрастанию (+) или по убыванию (-)?"<< endl;
                cin >> desk;
                if (desk == '+'){
                    for (const auto& product : products) {
                        print_product(product);
                    }
                    cout<<"\n"<<endl;
                }else if(desk == '-'){
                    reverse(products.begin(), products.end());
                    for (const auto& product : products) {
                        print_product(product);
                    }
                    cout<<"\n"<<endl;
                } else{
                    cout << "Введено неверное направление сортировки (+/-)\n"<< endl;
                }
                break;
            case 4:  // а здесь будем сортировать вставками
                insertionSort(products,
                [](const Check& a, const Check& b) { return a.get_name() < b.get_name(); });

                cout << "Сортировка по возрастанию (+) или по убыванию (-)?"<< endl;
                cin >> desk;
                if (desk == '+'){
                    for (const auto& product : products) {
                        print_product(product);
                    }
                    cout<<"\n"<<endl;
                }else if(desk == '-'){
                    reverse(products.begin(), products.end());
                    for (const auto& product : products) {
                        print_product(product);
                    }
                    cout<<"\n"<<endl;
                }else{
                    cout << "Введено неверное направление сортировки (+/-)\n"<< endl;
                }
                break;
            case 5:
                quickSort(products, 0, products.size() - 1,
                [](const Check& a, const Check& b) { return a.get_category() < b.get_category(); });

                cout << "Сортировка по возрастанию (+) или по убыванию (-)?"<< endl;
                cin >> desk;
                if (desk == '+'){
                    for (const auto& product : products) {
                        print_product(product);
                    }
                    cout<<"\n"<<endl;
                }else if(desk == '-'){
                    reverse(products.begin(), products.end());
                    for (const auto& product : products) {
                        print_product(product);
                    }
                    cout<<"\n"<<endl;
                }else{
                    cout << "Введено неверное направление сортировки (+/-)\n"<< endl;
                }
                break;
            case 6:
                cout << "Введите название товара: ";
                getline(cin, name);
                cout << "Введите категорию товара: ";
                getline(cin, category);
                search_w_name_categ(products, name, category);
                break;
            case 7:
                cout << "Введите цену, по которой будем искать: ";
                cin >> price;
                cin.ignore();
                equal_prices(Price2Products, price);
                break;
            default:
                cerr << "Некорректное действие. Пожалуйста, выберите действие из списка.\n";
                break;
        }
    }
    return 0;
}