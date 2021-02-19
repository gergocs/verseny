//g++ main.cpp -l sqlite3

#include <iostream>
#include <sqlite3.h>
#include <thread>

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

    const string success = "done";
    const string error = "error";
    const string closed = "closed";
    string tmp;

    const string query = "SELECT * FROM reports";
    const string last = "SELECT id FROM reports ORDER BY id DESC LIMIT 1";
    const string creater = "CREATE TABLE [IF NOT EXITS] reports ("
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
        mainEventLoop.post([conn, &server]() {
            clog << "Connection closed." << endl;
            clog << "There are now " << server.numConnections() << " open connections." << endl;
        });
    });

    server.message("message", [&mainEventLoop, &server](const ClientConnection& conn, const Json::Value &args) {
        mainEventLoop.post([conn, args, &server]() {
            clog << "message handler on the main thread" << endl;
            clog << "Message payload:" << endl;
            cout<<"Alma"<<endl;
            for (const auto& key : args.getMemberNames()) {
                clog << "\t" << key << ": " << args[key].asString() << endl;
            }
            server.sendMessage(conn, "message", args);
        });
    });

    thread serverThread([&server]() {
        server.run(PORT_NUMBER);
    });
    thread inputThread([&server, db, &query, &last, &exit, &messaggeError]() {
        string input = "ready";
        while (true) {
            gCounter = 0;
            gData = "{ \"reports\": [";
            cout<<"alma"<<endl;
            int exit2 = sqlite3_exec(db, query.c_str(), callback, nullptr, nullptr);
            if (exit2 != SQLITE_OK) {
                cout << "Error " << sqlite3_errmsg(db) << endl;
                cout << exit2 << endl;
                return -1;
            } else {
                cout << "DB opened" << endl;
            }
            cout<<gCounter<<endl;
            gData[gData.length()-1] = ' ';

            gData += "]}";
            cout<<gData<<endl;
            Json::Value payload;
            payload["input"] = input;
            server.broadcastMessage("message",payload);
            this_thread::sleep_for(chrono::seconds(1));
            server.message("message", [&db, &last, &exit, &messaggeError, &server](const ClientConnection& conn, const Json::Value &args) {
                string switcher = args["input"].asString();

                if(switcher == closer){
                    cout<<"Alma"<<endl;
                }else if(switcher == json){
                    Json::Reader reader;
                    reader.parse(gData, gJson);
                    server.broadcastMessage("message", gJson);
                }else if(switcher == inserter){

                    unsigned int counter = 0;

                    size_t found;

                    sqlite3_exec(db, last.c_str(), callback, nullptr, nullptr);

                    found = gData.find(R"("id": ")");
                    string str;

                    if (found != string::npos) {
                        size_t found2 = gData.find("\"\n");
                        for (unsigned int i = found + 7; i < found2; i++) {
                            str += gData[i];
                        }
                        counter = stoi(str);
                        counter++;
                    }

                    string insert = "INSERT INTO reports VALUES(";
                    insert += to_string(counter);
                    insert += ",\"";
                    insert += args["name"].asString(); + "\",\"" + args["class"].asString(); + "\", \"" + args["place"].asString() + "\", ";
                    insert += args["type"].asString();
                    insert += ",";
                    insert += args["current"].asString();
                    insert += ",";
                    insert += args["cprotect"].asString();
                    insert += ",";
                    insert += args["good"].asString();
                    insert += ",";
                    insert += args["resistance"].asString();
                    insert += ",\"";
                    insert += args["final"].asString();
                    insert += "\");";
                    cout << insert << endl;
                    exit = sqlite3_exec(db, insert.c_str(), nullptr, nullptr, &messaggeError);
                    if (exit != SQLITE_OK) {
                        cerr << "Error Insert" << endl;
                        sqlite3_free(messaggeError);
                    } else {
                        cout << "Done" << endl;
                    }


                    server.broadcastMessage("message",gJson);
                }else if(switcher == deleter){
                    string sql = "DELETE FROM reports WHERE id = " + args["id"].asString() + ";";

                    exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &messaggeError);

                    if (exit != SQLITE_OK) {
                        cerr << " ERROR DELETE" << endl;
                        sqlite3_free(messaggeError);
                    } else {
                        cout << "deleted " << args["id"].asString() << endl;
                    }
                }else{
                    cout<<switcher<<endl;
                }

            });
        }
    });

    asio::io_service::work work(mainEventLoop);
    mainEventLoop.run();

    sqlite3_close(db);

    return 0;
}