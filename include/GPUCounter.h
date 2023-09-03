//
// Created by Pengfei Gao on 2020/12/13.
//

#ifndef SAFELANG_GPUCOUNTER_H
#define SAFELANG_GPUCOUNTER_H

#include "GPUParser.h"
#include "GPUcounting_t.h"

class GPUCounter {
public:
    static pair<bool, double> GPUCount(vector<ThreeAddressNodePtr>& nodeVec, int bitlength) {
        string result = "";
        set<string> randoms;
        set<string> keys;
        set<string> plains;
        string r;
        string k;
        string p;

        list<string> exprs;

        string saving = "";

        vector<string> savingV;

        for(auto node : nodeVec) {
            result += GPUParser::parseNodeForCounting(node) + "\n\n";
            string exp = GPUParser::parseNodeForCounting(node);
            exprs.push_back(exp);

            set<string> tempRandom;
            ThreeAddressNode::getRandom(node, tempRandom);
            set<string> newtempRandom;
            for(auto ele : tempRandom)
                newtempRandom.insert(GPUParser::dealNodeName(ele));
            randoms.insert(newtempRandom.begin(), newtempRandom.end());

            set<string> tempKey;
            ThreeAddressNode::getKeys(node, tempKey);
            set<string> newtempKeys;
            for(auto ele : tempKey)
                newtempKeys.insert(GPUParser::dealNodeName(ele));
            keys.insert(newtempKeys.begin(), newtempKeys.end());

            set<string> tempPlains;
            ThreeAddressNode::getPublics(node, tempPlains);
            set<string> newtempPlains;
            for(auto ele : tempPlains)
                newtempPlains.insert(GPUParser::dealNodeName(ele));
            plains.insert(newtempPlains.begin(), newtempPlains.end());
        }

        for(string ra : randoms)
            r += ra + " ";
        r += "\n";

        for(string ra : keys)
            k += ra + " ";
        k += "\n";

        for(string ra : plains)
            p += ra + " ";
        p += "\n";

        cout <<  result + r + k + p;

        ExpTree2::gpuFunction(exprs, plains, randoms, keys, bitlength);

        system("nvcc -Wno-deprecated-gpu-targets ct.cu && ./a.out > report.txt");
        stringstream ss;
        FILE *fp;
        char buf[20] = {0};
        fp = fopen("report.txt", "r");

        if (NULL == fp) {
            perror("popen error!\n");
        }


        fgets(buf, 20, fp);

        bool res;
        ss << buf;
        ss >> res;

        memset(buf, 0, sizeof(buf));
        fgets(buf, 20, fp);

        double time;
        ss << buf;
        ss >> time;
        pclose(fp);

        cout << res << endl;
        cout << time << endl;
        system("rm a.out report.txt");
        return make_pair(res, time);

    }

};


#endif //SAFELANG_GPUCOUNTER_H
