#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <sstream>

using namespace std;

class Valve {
public:
    bool valveOpen;
    //! Valve sinifinin olusturucu fonksiyonu
    Valve() :valveOpen(false) {}
};

class FuelTank {
private:
    //! FuelTank sýnýfý için private ogeler
    bool isBroken;
    bool isConnected;
    //! FuelTank sinifinin bir uyesi olarak olusturulan valve nesnesi
    Valve valve;

public:
    //! Baska siniflar tarafindan da erisilebilecek public uyeler
    int tankID;
    double capacity;
    double fuelQuantity;

    //! FuelTank sýnýfýnýn oluþturucu fonksiyonu
    FuelTank(int id, double cap, Valve valve) : tankID(id), capacity(cap), fuelQuantity(0), isBroken(false), isConnected(false), valve(valve) {}

    void openValve() {
        valve.valveOpen = true;
    }

    void closeValve() {
        valve.valveOpen = false;
    }

    void breakTank() {
        isBroken = true;
    }

    void repairTank() {
        isBroken = false;
    }

    void connectTank() {
        isConnected = true;
    }

    void disconnectTank() {
        isConnected = false;
    }

    //! bu fonksiyonlar ile private üyelere friend kullanimi gerekmeden erisim saglaniyor
    bool isValveOpen() const {
        return valve.valveOpen;
    }

    bool isTankBroken() const {
        return isBroken;
    }

    bool isTankConnected() const {
        return isConnected;
    }
};

class Engine {
private:
    //! engine sýnýfý için private ögeler
    double fuelQuantity;
    const double fuel_per_second;
    bool status;
    int runTime;
    vector<FuelTank> operationalTanks;
    //! engine sýnýfýnýn oluþturucu fonksiyonu
    Engine() : fuelQuantity(0), fuel_per_second(5.5), status(false), runTime(0) {}

public:

    static Engine& getInstance() {
        static Engine instance;
        return instance;
    }

    Engine(const Engine&) = delete;
    void operator=(const Engine&) = delete;


    //! motorun çalýþma süresini kontrol eden fonksiyon
    int incRunTime(int pssd) {
        if (status) {
            runTime += pssd;
        }
        return runTime;
    }
    //! toplam yakit tüketimini hesaplayan fonksiyon
    double ttlFuelConsumed() {
        return (runTime * fuel_per_second);
    }

    double interFuelQuantity() {
        return fuelQuantity;
    }

    bool isRunning() {
        return status;
    }

    void addFuel(double quan) {
        fuelQuantity += quan;
    }

    void removeFuel(double quan) {
        fuelQuantity -= quan;
    }

    //! gerçekleþen iþlemlerde kullanaýlan yakýtý ayarlamak için
    bool useFuel(int time) {
        if (status) {
            if ((fuelQuantity - (time * fuel_per_second)) < 0) {
                return false;
            }
            else {
                fuelQuantity -= (time * fuel_per_second);
                return true;
            }
        }
    }

    //! motorun calýsma durumunu gerçekleþtirmek amaçli
    vector<FuelTank>& startEngine(const vector<FuelTank>& tanks) {
        bool canStart = false;
        for (const auto& tank : tanks) {
            if (tank.isValveOpen() && !tank.isTankBroken() && tank.isTankConnected() && tank.fuelQuantity >= 20) {
                operationalTanks.push_back(tank);
                canStart = true;
            }
            else {
                continue;
            }
        }

        if (canStart) {
            status = true;
        }

        return operationalTanks;
    }

    //! motorun durdurulma durumunu kontrol amacli
    void stopEngine() {
        if (status) {
            status = false;
        }
    }

};

class FuelTankManager {
private:
    //! Yakit tanklarini saklamak amacli vektor
    vector<FuelTank> fuelTanks;
    int newTankID = 0;
    int ttlFuelTanks = 0;
    double ttlExtFuelQuantity = 0;

public:
    //! yeni tank eklemek amacli
    void AddFuelTank(double capacity) {
        Valve valve;
        FuelTank newTank(newTankID++, capacity, valve);
        fuelTanks.push_back(newTank);
    }
    //! secili tanki cikarmak amacli
    void RemoveFuelTank(int tankID) {
        auto it = find_if(fuelTanks.begin(), fuelTanks.end(), [tankID](const FuelTank& tank) {
            return tank.tankID == tankID;
            });

        if (it != fuelTanks.end()) {
            fuelTanks.erase(it);
        }
        else {
        }
    }

    //! dis tanklardaki toplam yakiti bulmak amacli
    double TtlExtFuelQuantity() {
        double ttlExtFuel = 0;
        for (const auto& tank : fuelTanks) {
            ttlExtFuel += tank.fuelQuantity;
        }
        return ttlExtFuel;
    }

    //! sistemdeki tanklari listelemek amacli
    vector<FuelTank>& ListFuelTanks() {
        for (const auto& tank : fuelTanks) {
        }
        return fuelTanks;
    }

    //! motora bagli tanklari listeleme amacli
    void ListConnectedFuelTanks() {
        for (const auto& tank : fuelTanks) {
            if (tank.isTankConnected()) {
            }
            else {
                continue;
            }
        }
    }

    //! secili tanka yakit eklemek amacli
    bool FillTank(int tankID, double quan) {
        auto it = find_if(fuelTanks.begin(), fuelTanks.end(), [tankID](const FuelTank& tank) {
            return tank.tankID == tankID;
            });

        if (it != fuelTanks.end()) {
            if (it->fuelQuantity + quan <= it->capacity) {
                it->fuelQuantity += quan;
                ttlExtFuelQuantity += quan;
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }
    }

    //! toplam yakit tanki sayisini yazdirmak amacli
    int PrintFuelTankCount() {
        return fuelTanks.size();
    }

    //! tankin vanasini acmak amacli
    void OpenTankFuelValve(int tankID) {
        for (auto& tank : fuelTanks) {
            if (tank.tankID == tankID) {
                tank.openValve();
                return;
            }
        }
    }

    //! tankin vanasini kapatmak amacli
    void CloseTankFuelValve(int tankID) {
        for (auto& tank : fuelTanks) {
            if (tank.tankID == tankID) {
                tank.closeValve();
                return;
            }
        }
    }

    //! tanki bozmak amacli
    void BreakFuelTank(int tankID) {
        for (auto& tank : fuelTanks) {
            if (tank.tankID == tankID) {
                tank.breakTank();
                return;
            }
        }
    }

    //! tanki tamir etmek amacli
    void RepairFuelTank(int tankID) {
        for (auto& tank : fuelTanks) {
            if (tank.tankID == tankID) {
                tank.repairTank();
                return;
            }
        }
    }

    //! belli bir sure beklemek amacli
    void Wait(int sec) {
        this_thread::sleep_for(chrono::seconds(sec));
    }
};

class FileWriter {
public:
    //! motorun calisma komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void startEngine(ofstream& outputFile, bool isb, bool isr) {
        if (isb) {
            outputFile << "Engine Already Running" << endl;
        }
        else {
            if (isr) {
                outputFile << "Engine Started" << endl;               
            }
            else {
                outputFile << "Engine Couldn't Start. Check Valves and Fuel Tanks" << endl;
            }
        }
    }
    //! motorun durma komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void stopEngine(ofstream& outputFile, bool iso, bool check) {
        if (iso) {
            outputFile << "Engine is Already Off" << endl;
        }
        else {
            outputFile << "Engine Stopped" << endl;
            if (!check) {
                outputFile << "Fuel Couldn't Transferred to External Tank due to Capacity Problem" << endl;
            }
        }
    }
    //! tank ekleme komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void addTank(ofstream& outputFile, double cap) {
        outputFile << "Fuel Tank Added with " << cap << " Capacity" << endl;
    }
    //! tank bilgisi komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void printTankInfo(ofstream& outputFile, bool isf, int tankID, double cap, double quantity, bool isc, bool isbr, bool isop) {
        if (isf) {
            outputFile << "Tank " << tankID + 1 << " " << cap << " Capacity" << endl;
            outputFile << "       " << quantity << " Fuel" << endl;
            if (isc) {
                outputFile << "       is Connected" << endl;
            }
            else {
                outputFile << "       is Not Connected" << endl;
            }
            if (isbr) {
                outputFile << "       is Broken" << endl;
            }
            else {
                outputFile << "       is Repaired" << endl;
            }
            if (isop) {
                outputFile << "       Valve Open" << endl;
            }
            else {
                outputFile << "       Valve Closed" << endl;
            }
        }
        else {
            outputFile << "Tank Couldn't Found" << endl;
        }
    }
    //! tank doldurma komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void fillTank(ofstream& outputFile, int tankID, double cap, bool ise) {

        if (ise) {
            outputFile << "Added " << cap << " Fuel to Tank " << tankID + 1 << endl;
        }
        else {
            outputFile << "There is Not Enough Capacity Left or Tank Doesn't Exist" << endl;
        }
    }
    //! toplam tank sayisi komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void tankCount(ofstream& outputFile, int count) {
        outputFile << "There is " << count << " tanks in system" << endl;
    }
    //! toplam yakit komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void ttlFuel(ofstream& outputFile, double ttl) {
        outputFile << "Total Fuel in System: " << ttl << endl;
    }
    //! geri yakit verme komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void giveBackFuel(ofstream& outputFile, bool check, bool ise) {
        if (!ise) {
            outputFile << "There is not Enough Fuel in Internal Tank" << endl;
        }
        else {
            if (!check) {
                outputFile << "Fuel Couldn't Transferred to External Tank due to Capacity Problem" << endl;
            }
            else {
                outputFile << "Fuel Transferred to External Tank" << endl;
            }
        }
    }
    //! vana acma komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void openValve(ofstream& outputFile, int tankID, bool check) {
        if (!check) {
            outputFile << "Tank Couldn't Found" << endl;
        }
        else {
            outputFile << "Tank Valve Opened for Tank " << tankID + 1 << endl;
        }
    }
    //! vana kapama komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void closeValve(ofstream& outputFile, int tankID, bool check) {
        if (!check) {
            outputFile << "Tank Couldn't Found" << endl;
        }
        else {
            outputFile << "Tank Valve Closed for Tank " << tankID + 1 << endl;
        }
    }
    //! tank bozma komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void breakTank(ofstream& outputFile, int tankID, bool check) {
        if (!check) {
            outputFile << "Tank Couldn't Found" << endl;
        }
        else {
            outputFile << "Tank " << tankID + 1 << " is Now Broken" << endl;
        }
    }
    //! tank tamir etme komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void repairTank(ofstream& outputFile, int tankID, bool check) {
        if (!check) {
            outputFile << "Tank Couldn't Found" << endl;
        }
        else {
            outputFile << "Tank " << tankID + 1 << " is Now Repaired" << endl;
        }
    }
    //! tank baglama komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void connectTank(ofstream& outputFile, int tankID, bool check) {
        if (!check) {
            outputFile << "Tank Couldn't Found" << endl;
        }
        else {
            outputFile << "Tank " << tankID + 1 << " is Now Connected" << endl;
        }
    }
    //! tank ayirma komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void disconnectTank(ofstream& outputFile, int tankID, bool check) {
        if (!check) {
            outputFile << "Tank Couldn't Found" << endl;
        }
        else {
            outputFile << "Tank " << tankID + 1 << " is Now Disconnected" << endl;
        }
    }
    //! toplam yakilan yakit komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void ttlConsumedFuel(ofstream& outputFile, double ttlFuel) {
        outputFile << "Total Fuel Consumed: " << ttlFuel << endl;
    }
    //! bekleme komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void wait(ofstream& outputFile, double time) {
        outputFile << "Waiting for " << time << " Seconds" << endl;
    }
    //! tank silme komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void removeTank(ofstream& outputFile, int tankID, bool isf) {
        if (!isf) {
            outputFile << "Tank Couldn't Found" << endl;
        }
        else {
            outputFile << "Fuel Tank " << tankID + 1 << " Removed" << endl;
        }
    }
    //! gecersiz komut gelmesi durumunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void invalid(ofstream& outputFile) {
        outputFile << "Invalid Command" << endl;
    }
    //! tank listeleme komutunda verilmesi gereken ciktilari ayarlayan fonksiyon
    void listTanks(ofstream& outputFile, int tankID, double capacity) {
        outputFile << "Tank " << tankID + 1 << " with " << capacity << " Capacity" << endl;
    }
};

class Command {
private:
    string command;
public:
    void commandCreater(string str) {
        command = str;
    }
};

class FileReader {
private:
    Command command;
public:
    string lineEditer(ifstream& inputFile) {
        string line;
        while (true) {
            if (!getline(inputFile, line)) {
                //! dosya sonuna gelindiginde veya okuma hatasý olustugunda islemi sonlandýr
                return "";
            }

            //! eger satýr bossa yeni bir satira geciyor
            if (line.empty()) {
                continue;
            }

            //! ';' ve sonrasini siliyor
            while (1) {
                if (line.back() == ';') {
                    line.pop_back();
                    return line;
                }
                else {
                    line.pop_back();
                }
            }
        }
    }
    //! toplam komut sayisini hesaplayan fonksiyon
    int lineCount(ifstream& inputFile) {
        string line;
        int count = 0;
        while (getline(inputFile, line)) {
            if (!line.empty()) {
                count++;
            }
        }
        inputFile.close();
        return count;
    }
};

class CommandHolder {
private:
    //! CommandHolder sinifinin private üyeleri
    vector<string> commandvec;
    FileReader fileReader;
public:
    //! komutlari bir vektore atan fonksiyon
    void listMaker(string fileName) {
        ifstream inputFile(fileName);
        if (!inputFile.is_open()) {
            cerr << "File couldn't opened!" << endl;
        }
        int count = fileReader.lineCount(inputFile);
        ifstream InputFile(fileName);
        for (int i = 0; i < count; i++) {
            commandvec.push_back(fileReader.lineEditer(InputFile));
        }
    }
    //! komut listesini donduren fonksiyon
    vector<string> commandList() const {
        return commandvec;
    }
};

class CommandProcesor {
private:
    FuelTankManager& fuelTankManager;
    FileWriter& fileWriter;
    CommandHolder commandHolder;
public:
    CommandProcesor(FuelTankManager& fuelTankManager, FileWriter& fileWriter) : fuelTankManager(fuelTankManager), fileWriter(fileWriter) {}

    void processCommand(ofstream& outputFile, vector<string> commands) {
        
        Engine& engine = Engine::getInstance();

        for (const string& commandLine : commands) {
            cout << commandLine << endl;
            istringstream iss(commandLine);
            string command;
            iss >> command;
            cout << command << endl;

            if (engine.isRunning()) {
                if (!engine.useFuel(1)) {
                    bool check = false;
                    //! ana tanka yakýt ver
                    for (auto& tank : fuelTankManager.ListFuelTanks()) {
                        if (tank.isTankConnected() && !tank.isTankBroken() && tank.isValveOpen()) {
                            if (tank.fuelQuantity >= 20) {
                                engine.addFuel(20);
                                tank.fuelQuantity -= 20;
                                check = true;
                            }
                            else {
                                continue;
                            }
                        }
                        else {
                            continue;
                        }
                    }
                    if (check) {
                        engine.useFuel(1);
                    }
                    else {
                        engine.stopEngine();
                        outputFile << "Engine Stopped" << endl;
                    }
                }
                engine.incRunTime(1);
            }
            //! tank ekleme komutunda yapilmasi gereken islemler
            if (command == "add_fuel_tank") {
                double cap;
                iss >> cap;
                cout << cap << endl;
                fuelTankManager.AddFuelTank(cap);
                fileWriter.addTank(outputFile, cap);
            }
            //! motor calistirma komutunda yapilmasi gereken islemler
            else if (command == "start_engine") {
                bool isb = false;
                bool isr = false;
                if (engine.isRunning()) {
                    isb = true;
                }
                else {
                    engine.startEngine(fuelTankManager.ListFuelTanks());
                    if (engine.isRunning()) {
                        for (auto& tank : fuelTankManager.ListFuelTanks()) {
                            if (tank.isTankConnected() && !tank.isTankBroken() && tank.isValveOpen() && tank.fuelQuantity >= 20) {
                                engine.addFuel(20);
                                tank.fuelQuantity -= 20;
                                isr = true;
                            }
                            else {
                                continue;
                            }
                        }
                    }
                }
                fileWriter.startEngine(outputFile, isb, isr);
            }
            //! motor durdurma komutunda yapilmasi gereken islemler
            else if (command == "stop_engine") {
                bool iso = false;
                bool check = false;
                if (!engine.isRunning()) {
                    iso = true;
                }
                else {
                    engine.stopEngine();
                    for (auto& tank : fuelTankManager.ListFuelTanks()) {
                        if (!tank.isTankBroken() && tank.isTankConnected() && tank.isValveOpen()) {
                            if ((tank.fuelQuantity + engine.interFuelQuantity()) <= tank.capacity) {
                                tank.fuelQuantity += engine.interFuelQuantity();
                                engine.removeFuel(engine.interFuelQuantity());
                                check = true;
                                break;
                            }
                            else {
                                continue;
                            }
                        }
                    }

                }
                fileWriter.stopEngine(outputFile, iso, check);
            }
            //! tank bilgisi yazdirma komutunda yapilmasi gereken islemler
            else if (command == "print_tank_info") {
                bool isf = false;
                bool isc = false;
                bool isbr = false;
                bool isop = false;
                double cap = 0;
                double quantity = 0;
                int tankID;
                iss >> tankID;
                cout << tankID << endl;
                tankID -= 1;
                vector<FuelTank> tank = fuelTankManager.ListFuelTanks();
                auto it = find_if(tank.begin(), tank.end(), [tankID](const FuelTank& tank1) {
                    return tank1.tankID == tankID;
                    });
                if (it != tank.end()) {
                    cap = it->capacity;
                    quantity = it->fuelQuantity;
                    isf = true;
                    if (it->isTankConnected()) {
                        isc = true;
                    }
                    if (it->isTankBroken()) {
                        isbr = true;
                    }
                    if (it->isValveOpen()) {
                        isop = true;
                    }
                }
                fileWriter.printTankInfo(outputFile, isf, tankID, cap, quantity, isc, isbr, isop);
            }
            //! tank doldurma komutunda yapilmasi gereken islemler
            else if (command == "fill_tank") {
                double capacity;
                int tankID;
                bool ise = false;
                iss >> tankID >> capacity;
                cout << tankID << " " << capacity << endl;
                tankID -= 1;
                if (fuelTankManager.FillTank(tankID, capacity)) {
                    ise = true;
                }
                fileWriter.fillTank(outputFile, tankID, capacity, ise);
            }
            else if (command == "print_fuel_tank_count") {
                fileWriter.tankCount(outputFile, fuelTankManager.PrintFuelTankCount());
            }
            else if (command == "print_total_fuel_quantity") {
                fileWriter.ttlFuel(outputFile, engine.interFuelQuantity() + fuelTankManager.TtlExtFuelQuantity());
            }
            //! yakit geri verme komutunda yapilmasi gereken islemler
            else if (command == "give_back_fuel") {
                double capacity;
                iss >> capacity;
                cout << capacity << endl;
                bool check = false;
                bool ise = true;
                if (capacity > engine.interFuelQuantity()) {
                    ise = false;
                }
                else {
                    for (auto& tank : fuelTankManager.ListFuelTanks()) {
                        if (!tank.isTankBroken() && tank.isTankConnected() && tank.isValveOpen()) {
                            if ((tank.fuelQuantity + capacity) <= tank.capacity) {
                                tank.fuelQuantity += capacity;
                                engine.removeFuel(capacity);
                                check = true;
                                break;
                            }
                            else {
                                continue;
                            }
                        }
                    }
                }
                fileWriter.giveBackFuel(outputFile, check, ise);
            }
            //! vana acma komutunda yapilmasi gereken islemler
            else if (command == "open_valve") {
                bool check = false;
                int tankID;
                iss >> tankID;
                cout << tankID << endl;
                tankID -= 1;
                for (auto& tank : fuelTankManager.ListFuelTanks()) {
                    if (tank.tankID == tankID) {
                        tank.openValve();
                        check = true;
                        break;
                    }
                }
                fileWriter.openValve(outputFile, tankID, check);
            }
            //! vana kapama komutunda yapilmasi gereken islemler
            else if (command == "close_valve") {
                bool check = false;
                int tankID;
                iss >> tankID;
                cout << tankID << endl;
                tankID -= 1;
                for (auto& tank : fuelTankManager.ListFuelTanks()) {
                    if (tank.tankID == tankID) {
                        tank.closeValve();
                        check = true;
                        break;
                    }
                }
                fileWriter.closeValve(outputFile, tankID, check);
            }
            //! tank kirma komutunda yapilmasi gereken islemler
            else if (command == "break_fuel_tank") {
                bool check = false;
                int tankID;
                iss >> tankID;
                cout << tankID << endl;
                tankID -= 1;
                for (auto& tank : fuelTankManager.ListFuelTanks()) {
                    if (tank.tankID == tankID) {
                        tank.breakTank();
                        check = true;
                        break;
                    }
                }
                fileWriter.breakTank(outputFile, tankID, check);
            }
            //! tank tamir etme komutunda yapilmasi gereken islemler
            else if (command == "repair_fuel_tank") {
                bool check = false;
                int tankID;
                iss >> tankID;
                cout << tankID << endl;
                tankID -= 1;
                for (auto& tank : fuelTankManager.ListFuelTanks()) {
                    if (tank.tankID == tankID) {
                        tank.breakTank();
                        check = true;
                        break;
                    }
                }
                fileWriter.repairTank(outputFile, tankID, check);
            }
            //! tank baglama komutunda yapilmasi gereken islemler
            else if (command == "connect_fuel_tank_to_engine") {
                bool check = false;
                int tankID;
                iss >> tankID;
                cout << tankID << endl;
                tankID -= 1;
                for (auto& tank : fuelTankManager.ListFuelTanks()) {
                    if (tank.tankID == tankID) {
                        tank.connectTank();
                        check = true;
                        break;
                    }
                }
                fileWriter.connectTank(outputFile, tankID, check);
            }
            //! tank ayirma komutunda yapilmasi gereken islemler
            else if (command == "disconnect_fuel_tank_from_engine") {
                bool check = false;
                int tankID;
                iss >> tankID;
                cout << tankID << endl;
                tankID -= 1;
                for (auto& tank : fuelTankManager.ListFuelTanks()) {
                    if (tank.tankID == tankID) {
                        tank.disconnectTank();
                        check = true;
                        break;
                    }
                }
                fileWriter.disconnectTank(outputFile, tankID, check);
            }
            //! tank listeleme komutunda yapilmasi gereken islemler
            else if (command == "list_fuel_tanks") {
                for (const auto& tank : fuelTankManager.ListFuelTanks()) {
                    fileWriter.listTanks(outputFile, tank.tankID, tank.capacity);
                }
            }
            //! bagli tank listeleme komutunda yapilmasi gereken islemler
            else if (command == "list_connected_tanks") {
                for (const auto& tank : fuelTankManager.ListFuelTanks()) {
                    if (tank.isTankConnected()) {
                        fileWriter.listTanks(outputFile, tank.tankID, tank.capacity);
                    }
                }
            }
            else if (command == "print_total_consumed_fuel_quantity") {
                fileWriter.ttlConsumedFuel(outputFile, engine.ttlFuelConsumed());
            }
            //! bekleme komutunda yapilmasi gereken islemler
            else if (command == "wait") {
                double capacity;
                bool check = false;
                iss >> capacity;
                cout << capacity << endl;
                if (engine.isRunning()) {
                    if (!engine.useFuel(capacity - 1)) {
                        // ana tanka yakýt ver
                        for (auto& tank : fuelTankManager.ListFuelTanks()) {
                            if (tank.isTankConnected() && !tank.isTankBroken() && tank.isValveOpen()) {
                                if (tank.fuelQuantity >= 20) {
                                    engine.addFuel(20);
                                    tank.fuelQuantity -= 20;
                                    check = true;
                                }
                                else {
                                    continue;
                                }
                            }
                            else {
                                continue;
                            }
                        }
                        if (check) {
                            engine.useFuel(capacity - 1);
                        }
                        else {
                            engine.stopEngine();
                        }
                    }
                    engine.incRunTime(capacity - 1);
                }
                fuelTankManager.Wait(capacity);
                fileWriter.wait(outputFile, capacity);
            }
            //! tank silme komutunda yapilmasi gereken islemler
            else if (command == "remove_fuel_tank") {
                int tankID;
                bool isf = true;
                iss >> tankID;
                cout << tankID << endl;
                tankID -= 1;
                if (tankID<0 || tankID>fuelTankManager.PrintFuelTankCount()) {
                    isf = false;
                }
                else {
                    fuelTankManager.RemoveFuelTank(tankID);
                }
                fileWriter.removeTank(outputFile, tankID, isf);
            }
            else if (command == "stop_simulation") {
                outputFile.close();
                return;
            }
            else {
                fileWriter.invalid(outputFile);
            }
        }      
    }
};

class Simulation {
private:
    //! simulation sinifinin private uyeleri
    CommandProcesor commandProcessor;
    string command;
    ofstream outputFile;
    CommandHolder commandHolder;
public:
    //! simulation sinifinin olusturucu fonksiyonu
    Simulation(CommandProcesor commandProcessor) : commandProcessor(commandProcessor) {}
    void simulation(string inFileName, string outFileName) {
        ifstream inputFile(inFileName);
        outputFile.open(outFileName);
        //! girdi dosyasinin acilmama durumu
        if (!inputFile.is_open()) {
            cerr << "Unable to open input file: " << inFileName << endl;
            return;
        }
        //! cikti dosyasinin acilmama durumu
        if (!outputFile.is_open()) {
            cerr << "Unable to open output file: " << outFileName << endl;
        }
        commandHolder.listMaker(inFileName);
        vector<string> commands = commandHolder.commandList();
        commandProcessor.processCommand(outputFile, commands);
    }
};

int main()   {
    string inFile, outFile;
    //! dosya isimlerinin alinmasi

    cout << "Enter input file name: ";
    cin >> inFile;

    cout << "Enter output file name: ";
    cin >> outFile;

    //! siniflarin nesnelerinin olusturulmasi
    FuelTankManager fuelTankManager;
    FileWriter fileWriter;
    CommandProcesor commandProcessor(fuelTankManager, fileWriter);
    Simulation simulation(commandProcessor);

    simulation.simulation(inFile, outFile);

    return 0;
}