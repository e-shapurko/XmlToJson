#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <fstream>
#include <condition_variable>
#include "TinyXML/tinyxml.h"
#include <iostream>

class Convertor {
private:
    class XmlParser {
    public:
        explicit XmlParser(const std::string path){readXml(path);}
        TiXmlDocument getDoc(){
            return doc_;
        }
    private:
        void readXml(const std::string path){
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
        void writeJson(std::unique_ptr<TiXmlDocument> doc){
            std::string jsonData;
            prepareJsonData(jsonData, "{");
            prepareJsonData(jsonData, "\"subscribers\": [");
            extractNodes(doc.get()->FirstChildElement(), jsonData);
            prepareJsonData(jsonData, "}");
            prepareJsonData(jsonData, "]");
            prepareJsonData(jsonData, "}");
            std::cout << "jsonData" <<std::endl;
        }

    private:
        void extractNodes(TiXmlElement * node, std::string & data) {
            for (TiXmlElement *noteItem = node;
                 noteItem != nullptr;
                 noteItem = noteItem->NextSiblingElement())
            {
                if (!noteItem->FirstAttribute())
                {
                    extractNodes(noteItem->FirstChildElement(), data);
                } else {
                    extractAttributes(noteItem->FirstAttribute(), data);
                }
            }
        }

        void extractAttributes(TiXmlAttribute * attr, std::string & data) {
            std::string delimetr;
            for (TiXmlAttribute *attrItem = attr;
                 attrItem != nullptr;
                 attrItem = attrItem->Next()){
                prepareJsonData(data, delimetr);
                prepareJsonData(data, "{");
                prepareJsonData(data,  quated(attrItem->Name()) + ": " + quated(attrItem->Value()));
                prepareJsonData(data, "}");
                delimetr = ",";
            }
        }

        void prepareJsonData(std::string & data, const std::string value){
            data = data + value + '\n';
        }

        std::string quated(const std::string value){
            return "\"" + value + "\"";
        }
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
