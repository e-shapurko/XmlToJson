#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <fstream>
#include <vector>
#include <memory>
#include "TinyXML/tinyxml.h"
#include <string>
#include "queue/queue.h"

class Convertor {
public:
    explicit Convertor(uint32_t queueSize): que(queueSize){}
    void readXml(const std::string path){
        std::string xml = readXmlFileToString(path);
        TiXmlDocument doc;
        doc.Parse(xml.c_str(), nullptr, TIXML_ENCODING_UTF8);
        que.push(std::make_shared<TiXmlElement>(*doc.FirstChildElement()));
    }

    void writeJson(){
        auto el = que.pop();
        std::string jsonData;
        prepareJsonData(jsonData, "{");
        prepareJsonData(jsonData, "\"subscribers\": [");
        extractNodes(el.get(), jsonData);
        prepareJsonData(jsonData, "}");
        prepareJsonData(jsonData, "]");
        prepareJsonData(jsonData, "}");
        //создание файла.............
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
    NodesQueue<TiXmlElement> que;
};

#endif // CONVERTOR_H
