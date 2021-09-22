#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <fstream>
#include <condition_variable>
#include "TinyXML/tinyxml.h"
#include <iostream>
#include <unistd.h>

class Convertor {
private:
    class XmlParser {
    public:
        explicit XmlParser(const std::string &path){readXml(path);}
        TiXmlDocument getDoc(){
            return doc_;
        }
    private:
        void readXml(const std::string &path){
            std::string xml = readXmlFileToString(path);
            doc_.Parse(xml.c_str(), nullptr, TIXML_ENCODING_UTF8);
        }

        std::string readXmlFileToString(const std::string &path) {
            std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
            if (!ifs) {
                return "";
            }
            std::ifstream::pos_type fileSize = ifs.tellg();
            ifs.seekg(0, std::ios::beg);
            std::vector<char> bytes(fileSize);
            ifs.read(bytes.data(), fileSize);
            return std::string(bytes.data(), fileSize);
        }
        TiXmlDocument doc_;
    };

    class JsonWriter {
    public:
        JsonWriter(): jsonData_(""){}

        void writeJson(std::unique_ptr<TiXmlDocument> doc){
            extractNodes(doc.get()->FirstChildElement());
            std::cout << jsonData_ <<std::endl;
            sleep(1);
        }

    private:
        void extractNodes(TiXmlElement * node) {
            std::string delimetr = "";
            for (TiXmlElement *nodeItem = node;
                 nodeItem != nullptr;
                 nodeItem = nodeItem->NextSiblingElement())
            {
                concatinateJsonData(delimetr);
                concatinateJsonData("{");
                if (!nodeItem->FirstAttribute())
                {
                    if (!nodeItem->FirstChildElement()) {
                        concatinateJsonData(quated(nodeItem->Value()) + ": " + nodeItem->FirstChild()->Value());
                    } else {
                        concatinateJsonData(quated(nodeItem->Value()) + ": [");
                        extractNodes(nodeItem->FirstChildElement());
                        concatinateJsonData("]");
                    }
                } else {
                    extractAttributes(nodeItem->FirstAttribute());
                    extractNodes(nodeItem->FirstChildElement());
                }
                concatinateJsonData("}");
                delimetr = ",";
            }
        }

        void extractAttributes(TiXmlAttribute * attr) {
            std::string delimetr;
            for (TiXmlAttribute *attrItem = attr;
                 attrItem != nullptr;
                 attrItem = attrItem->Next()){
                concatinateJsonData(quated(attrItem->Name()) + ": " + quated(attrItem->Value()));
            }
        }

        void concatinateJsonData(const std::string &value){
            jsonData_ += value + '\n';
        }

        std::string quated(const std::string &value){
            return "\"" + value + "\"";
        }

        std::string jsonData_;
    };



public:
    explicit Convertor(unsigned maxQueueSize);

    void run(const std::vector<std::string>& fileNames);

private:
    static void consumerProc(Convertor& conv);
    static void producerProc(Convertor& conv, const std::vector<std::string>& files);

private:
    const unsigned maxSize_;
    std::mutex m_;
    std::condition_variable cv_;
    bool terminate_{false};
    std::queue<std::unique_ptr<TiXmlDocument>> queue_;
};
