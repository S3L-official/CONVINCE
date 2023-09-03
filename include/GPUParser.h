//
// Created by pengfei on 1/22/19.
//

#ifndef EUROCRYPTO15_GPUPARSER_H
#define EUROCRYPTO15_GPUPARSER_H

#include <string>
#include "StateH.h"
using namespace std;


class GPUParser {
public:
    static string getFirstStringForCounting(ThreeAddressNodePtr n) {
        string s="";
        string leftName, rightName;
        if(n->getLhs() != NULL)
            leftName = dealNodeName((n->getLhs())->getNodeName());
        if(n->getRhs() != NULL)
            rightName = dealNodeName((n->getRhs())->getNodeName());


        if (n->getOp() == ASTNode::Operator::AND) {
            s.append("( &  " + leftName + "  " + rightName + "  ) ");
        }else if (n->getOp() == ASTNode::Operator::OR) {
            s.append("( |  " + leftName + "  " + rightName + "  ) ");
        }else if (n->getOp() == ASTNode::Operator::XOR) {
            s.append("( ^  " + leftName + "  " + rightName + "  ) ");
        }else if (n->getOp() == ASTNode::Operator::NOT) {
            s.append("( ~  " + leftName + "  " + rightName + "  ) ");
        } else if(n->getOp() == ASTNode::Operator::MINUS) {
            s.append("( -  " + leftName + "  " + rightName + "  ) ");
        } else if(n->getOp() == ASTNode::Operator::FFTIMES) {
            s.append("( !  " + leftName + "  " + rightName + "  ) ");
        } else if(n->getOp() == ASTNode::Operator::MUL) {
            s.append("( *  " + leftName + "  " + rightName + "  ) ");
        } else if(n->getOp() == ASTNode::Operator::ADD) {
            s.append("( +  " + leftName + "  " + rightName + "  ) ");
        } else if(n->getOp() == ASTNode::Operator::LSH) {
            s.append("( <<  " + leftName + "  " + rightName + "  ) ");
        } else if(n->getOp() == ASTNode::Operator::POW2) {
            s.append("( pow2  "  + leftName + " ) ");
        } else if(n->getOp() == ASTNode::Operator::SBOX) {
            s.append("( $  " + leftName + " ) ");
        }
        else {
            s.append("  " + leftName + "  " + rightName + "  ");
        }
        return s;
    }

    static string parseNodeForCounting(ThreeAddressNodePtr n) {
        string s = getFirstStringForCounting(n);
        if (n->getNodeType() == NodeType::RANDOM ||
            n->getNodeType() == NodeType::PRIVATE ||
            n->getNodeType() == NodeType::PUBLIC) {
            return " " + dealNodeName(n->getNodeName()) + " ";
        }

        if(n->getNodeType() == NodeType::CONSTANT) {
            string name = dealNodeName(n->getNodeName());
//            name.at(0) = '0';
            name.replace(0, 2, "??");
            return " " + name + " ";
        }

        if (n->getLhs() != NULL) {
            string ss = parseNodeForCounting(n->getLhs()) ;
//			if(ss == " x ") {
//				s = s.replace(s.find(" " + n.getLeftChild()->getName() + " ") + 1, n.getLeftChild()->getName().size(), ss);
//			} else {
//				s = s.replace(s.find(n.getLeftChild()->getName()), n.getLeftChild()->getName().size(), ss);
//			}

            s = s.replace(s.find(" " + dealNodeName(n->getLhs()->getNodeName()) + " "), dealNodeName(n->getLhs()->getNodeName()).size() + 2, ss);

        }
        if (n->getRhs() != NULL) {
            string ss = parseNodeForCounting(n->getRhs()) ;
//			if(ss == " x ") {
//				s = s.replace(s.find(" " + n.getLeftChild()->getName() + " ") + 1, n.getLeftChild()->getName().size(), ss);
//			} else {
//				s = s.replace(s.find(n.getRightChild()->getName()), n.getRightChild()->getName().size(), ss);
//			}

            s = s.replace(s.find(" " + dealNodeName(n->getRhs()->getNodeName()) + " "), dealNodeName(n->getRhs()->getNodeName()).size() + 2, ss);

        }
        return s;
    }

    static string dealNodeName(string nodeName) {
        for (int i = 0; i < nodeName.length(); ++i) {
            if (nodeName[i] == '@' || nodeName[i] == '(' || nodeName[i] == ')')
                nodeName[i] = '_';
        }
        return nodeName;
    }

};


#endif //EUROCRYPTO15_GPUPARSER_H
