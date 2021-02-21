//g++ main.cpp -l sqlite3

#include <iostream>
#include <sqlite3.h>
#include <thread>
#include <utility>

#include "external/asio/io_service.hpp"
#include "WebsocketServer.h"

#define database "rezdb.db"

#define PORT_NUMBER 8080

#define closer "close"
#define json "json"
#define inserter "insert"
#define deleter "delete"

using namespace std;

unsigned int gCounter;
string gData;

Json::Value gJson;

static int callback(void *data, int argc, char **argv, char **azColName) {
    gData += "{";

    for (int i = 0; i < argc; i++) {
        gData += "\"";
        gData += azColName[i];
        gData += "\":\"";
        gData += argv[i] ? argv[i] : "NULL";
        if (i != argc - 1) {
            gData += "\",";
        } else {
            gData += "\"";
        }

    }
    gData += "},";

    gCounter++;

    return 0;
}

int main() {
    sqlite3 *db;

    char *messaggeError;

    int exit;

    bool busy = false;

    string tmp;

    string previ;
    string prevd;

    const string success = "done";
    const string error = "error";
    const string closed = "closed";


    const string query = "SELECT * FROM reports";
    const string last = "SELECT id FROM reports ORDER BY id DESC LIMIT 1";
    const string creater = "CREATE TABLE IF NOT EXISTS reports ("
                           "'id' int PRIMARY KEY,"
                           "'name' text DEFAULT NULL,"
                           "'class' tinytext DEFAULT NULL,"
                           "'place' text DEFAULT NULL,"
                           "'type' int(11) DEFAULT NULL,"
                           "`current` int(11) DEFAULT 0,"
                           "`cprotect` tinyint(1) NOT NULL,"
                           "`good` tinyint(1) NOT NULL,"
                           "`resistance` int(11) NOT NULL,"
                           "`final` longtext NOT NULL"
                           ");";

    exit = sqlite3_open(database, &db);

    sqlite3_exec(db,creater.c_str(), nullptr, nullptr, &messaggeError);

    if (exit != SQLITE_OK) {
        cerr << "Error Create Table" << endl;
        sqlite3_free(messaggeError);
    }else{
        cout<<"done"<<endl;
    }

    sqlite3_exec(db, query.c_str(), callback, nullptr, nullptr);

    if (exit != SQLITE_OK) {
        cerr << "Error " << sqlite3_errmsg(db) << endl;
        cout << exit << endl;
        return -1;
    } else {
        cout << "DB opened" << endl;
    }

    asio::io_service mainEventLoop;
    WebsocketServer server;

    server.connect([&mainEventLoop, &server](const ClientConnection& conn) {
        mainEventLoop.post([conn, &server]() {
            clog << "Connection opened." << endl;
            clog << "There are now " << server.numConnections() << " open connections." << endl;
            server.sendMessage(conn, "hello", Json::Value());
        });
    });

    server.disconnect([&mainEventLoop, &server](ClientConnection conn) {
        mainEventLoop.post([move(std::move(conn)), &server]() {
            clog << "Connection closed." << endl;
            clog << "There are now " << server.numConnections() << " open connections." << endl;
        });
    });

    server.message("message", [&mainEventLoop, &server](const ClientConnection& conn, const Json::Value &args) {
        mainEventLoop.post([conn, args, &server]() {
            clog << "message handler on the main thread" << endl;
            clog << "Message payload:" << endl;
            for (const auto& key : args.getMemberNames()) {
                clog << "\t" << key << ": " << args[key].asString() << endl;
            }
            server.sendMessage(conn, "message", args);
        });
    });

    thread serverThread([&server]() {
        server.run(PORT_NUMBER);
    });
    thread inputThread([&server, &db, &query, &last, &exit, &messaggeError, &previ, &busy, &prevd]() {
        while (true) {
            gCounter = 0;

            gData = "{ \"reports\": [";

            int exit2 = sqlite3_exec(db, query.c_str(), callback, nullptr, nullptr);

            if(exit2 != SQLITE_OK){
                cout << "Error " << sqlite3_errmsg(db) << endl;
                cout << exit2 << endl;
            }else{
                cout << "I'm ready" << endl;
            }

            gData[gData.length()-1] = ' ';
            gData += "]}";

            this_thread::sleep_for (chrono::milliseconds(500));


            server.message("message", [&db, &last, &exit, &messaggeError, &previ, &prevd, &busy, &server](const ClientConnection& conn, const Json::Value &args) {
                string switcher = args["input"].asString();
                if (switcher == closer) {
                    cout << "Alma" << endl;
                }else if(switcher == "ping"){
                    Json::Value pong;
                    pong["input"] = "pong";
                    server.sendMessage(conn, "message",pong);
                }else if(switcher == "rbusy"){
                    if(busy){
                        gJson["input"] = "busy";
                    }else{
                        gJson["input"] = "nbusy";
                    }

                    server.sendMessage(conn, "message",gJson);
                }else if(switcher == json){
                    Json::Reader reader;
                    reader.parse(gData, gJson);
                    gJson["input"] = "data";
                    server.sendMessage(conn, "message", gJson);
                }else if(switcher == inserter){

                    busy = true;

                    unsigned int counter = 0;

                    gData = "{ \"reports\": [";

                    int exit3 = sqlite3_exec(db, last.c_str(), callback, nullptr, nullptr);

                    if(exit3 != SQLITE_OK){
                        cout << "Error " << sqlite3_errmsg(db) << endl;
                        cout << exit3 << endl;
                    }else{
                        cout << "I'm ready" << endl;
                    }

                    gData[gData.length()-1] = ' ';
                    gData += "]}";

                    cout<<gData<<endl;

                    size_t found = gData.find(R"({"id":")");

                    if(found != string::npos){
                        string str;
                        size_t found2 = gData.find("} ]}");
                        for(unsigned int i = found+7; i < found2; i++){
                            str += gData[i];
                        }
                        counter = stoi(str);
                    }

                    cout<<counter<<endl;

                    counter++;

                    string insert = "INSERT INTO reports VALUES(";

                    insert += to_string(counter);

                    string after;
                    after += ",\"";
                    after += args["name"].asString() + "\",\"" + args["class"].asString() + "\", \"" + args["place"].asString() + "\", ";
                    after += args["type"].asString();
                    after += ",";
                    after += args["current"].asString();
                    after += ",";
                    after += args["cprotect"].asString();
                    after += ",";
                    after += args["good"].asString();
                    after += ",";
                    after += args["resistance"].asString();
                    after += ",\"";
                    after += args["final"].asString();
                    after += "\");";

                    insert += after;

                    if(after != previ){
                        exit = sqlite3_exec(db, insert.c_str(), nullptr, nullptr, &messaggeError);

                        if (exit != SQLITE_OK) {
                            cout << "Error Insert" <<messaggeError<< endl;
                            sqlite3_free(messaggeError);
                        } else {
                            cout << "Done" << endl;
                        }

                        previ = after;
                    }

                    busy = false;

                }else if(switcher == deleter){
                    busy = true;

                    string sql = "DELETE FROM reports WHERE id = " + args["id"].asString() + ";";

                    cout<<sql<<endl;

                    if(sql != prevd) {

                        exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &messaggeError);

                        if (exit != SQLITE_OK) {
                            cout << " ERROR DELETE" << endl;
                            sqlite3_free(messaggeError);
                        } else {
                            cout << "deleted " << args["id"].asString() << endl;
                        }

                        prevd = sql;
                    }
                    busy = false;
                }else{
                    cout<<switcher<<endl;
                    this_thread::sleep_for(chrono::seconds(10));
                }

            });
        }
    });

    asio::io_service::work work(mainEventLoop);
    mainEventLoop.run();

    sqlite3_close(db);

    return 0;
}