//
// Created by Pengfei Gao on 2020/10/14.
//

#include "StateH.h"

ThreeAddressNodePtr StateH::top = make_shared<ThreeAddressNode>("top_node", nullptr, nullptr, ASTNode::Operator::NULLOP, NodeType::UNKNOWN);



bool StateH::directlyInclude(int limit) {
    if(limit < order)
        return false;
    if(std::includes(orginalAuxY.begin(), orginalAuxY.end(), originalValue.begin(), originalValue.end())) {
        for(auto ele : originalValue) {
            assert(saved.count(ele->getNodeName()) != 0);
            assert(copiedAuxYset.count(saved[ele->getNodeName()]) != 0);
                copiedProof.insert(copiedAuxYset[saved[ele->getNodeName()]]);
        }
        return true;
    } else {
        return false;
    }

}



const vector<set<ThreeAddressNodePtr>> &StateH::getOriginalY() const {
    return originalY;
}

const vector<DomExpressionPair> &StateH::getDomExpressions() const {
    return domExpressions;
}

const set<ThreeAddressNodePtr> &StateH::getCopiedKeyExpressions() const {
    return copiedKeyExpsAfterSimp;
}

