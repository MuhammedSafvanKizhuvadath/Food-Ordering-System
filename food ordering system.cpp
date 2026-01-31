#include <iostream>
#include <unordered_map>
#include <vector>
#include <iomanip>
#include <limits>
#include <string>

using namespace std;

static inline void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

struct MenuItem {
    int id;
    string name;
    double price;
    int stock;
    int prepTime;
};

struct OrderLine {
    int id;
    int qty;
};

class Input {
public:
    template<typename T>
    static bool read(T& v) {
        cin >> v;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return false;
        }
        return true;
    }

    static string readLine() {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string s;
        getline(cin, s);
        return s;
    }
};

class MenuRepository {
    unordered_map<int, MenuItem> items;

public:
    bool add(const MenuItem& m) {
        return items.emplace(m.id, m).second;
    }

    MenuItem* find(int id) {
        auto it = items.find(id);
        if (it == items.end()) return nullptr;
        return &it->second;
    }

    const unordered_map<int, MenuItem>& all() const {
        return items;
    }

    bool reduceStock(int id, int q) {
        auto it = items.find(id);
        if (it == items.end() || it->second.stock < q) return false;
        it->second.stock -= q;
        return true;
    }

    void display() const {
        cout << "\nID   Item                 Price     Stock  Prep\n";
        cout << "------------------------------------------------\n";
        for (auto& p : items) {
            auto& i = p.second;
            cout << left << setw(5) << i.id
                 << setw(20) << i.name
                 << "$" << setw(8) << fixed << setprecision(2) << i.price
                 << setw(7) << i.stock
                 << i.prepTime << "m\n";
        }
    }
};

class OrderService {
    MenuRepository& repo;

    vector<OrderLine> cart;
    unordered_map<int,int> popularity;

public:
    OrderService(MenuRepository& r):repo(r){}

    void addOrder(int id,int q){
        cart.push_back({id,q});
        popularity[id]+=q;
    }

    bool tryOrder(int id,int q){
        if(!repo.reduceStock(id,q)) return false;
        addOrder(id,q);
        return true;
    }

    void receipt(){
        double total=0;
        cout<<"\n------ RECEIPT ------\n";
        for(auto&o:cart){
            auto* it=repo.find(o.id);
            if(!it) continue;
            double c=it->price*o.qty;
            total+=c;
            cout<<it->name<<" x"<<o.qty<<" = $"<<fixed<<setprecision(2)<<c<<"\n";
        }
        cout<<"TOTAL: $"<<total<<"\n";
    }

    void popular(){
        int id=-1,maxv=0;
        for(auto&p:popularity){
            if(p.second>maxv){
                maxv=p.second;
                id=p.first;
            }
        }
        if(id!=-1){
            auto* i=repo.find(id);
            if(i) cout<<"\nMost popular: "<<i->name<<" ("<<maxv<<")\n";
        }
    }

    void clear(){
        cart.clear();
    }
};

class ManagerUI {
    MenuRepository& repo;

public:
    ManagerUI(MenuRepository& r):repo(r){}

    void panel(){
        int c;
        do{
            clearScreen();
            cout<<"\n1 Add\n2 Price\n3 Stock\n4 Prep\n5 Show\n0 Exit\nChoice: ";
            if(!Input::read(c)) continue;

            if(c==1){
                MenuItem m;
