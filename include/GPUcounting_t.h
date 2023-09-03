#ifndef GPUCOUNTING
#define GPUCOUNTING
#include <iostream>
#include <fstream>
#include<string>
#include<list>
#include<sstream>
#include<stack>
#include<set>
#include <cstdlib>
#include<map>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <cstring>
#include <memory>

using namespace std;


const int domain = 2;


class ExpTree2
{
	public:
		string data;
		ExpTree2 *left;
		ExpTree2 *right;
		ExpTree2(string d,  ExpTree2 *l=NULL, ExpTree2 *r=NULL ){

		    data=d;
		    left = l;
		    right = r;
//		    if(data == "")
//		        assert(false);
		};

		~ExpTree2(){
                //std::cout<<"Destructor"<<std::endl;
            }
		string print_tree()
		{
			if(data=="~")
			{
				return "~"+left->print_tree();
			}else if(data=="!")
			{
				return "gfmul ( "+left->print_tree()+" , "+right->print_tree()+" ) ";
			}else if(data=="$")
			{
				return "$ ( "+left->print_tree()+" ) ";
			}else if(data=="+")
			{
				return "add ( "+left->print_tree()+" , "+right->print_tree()+" ) ";
			}else if(data=="-")
			{
				return "sub ("+left->print_tree()+" , "+right->print_tree()+" ) ";
			}else if(data=="*")
			{
				return "mul ( "+left->print_tree()+" , "+right->print_tree()+" ) ";
			}else if(data==">>")
			{
				return "r_shift ( "+left->print_tree()+" , "+right->print_tree()+" ) ";
			}else if(data=="&"||data=="|"||data=="^"||data=="<<")
			{
				return " ( "+left->print_tree()+" "+data+" "+right->print_tree()+" ) ";
			}
			else if(data.find("pow")!=string::npos && left!=NULL)
			{
				return data+" ( "+left->print_tree()+" ) ";
			}else{
				return data;
			}

	}



    string printtree()
    {
        if(data=="~")
        {
            return "~"+left->printtree();
        }else if(data=="!")
        {
            return "gfmul ( "+left->printtree()+" , "+right->printtree()+" ) ";
        }else if(data=="$")
        {
            return "$ ( "+left->printtree()+" ) ";
        }else if(data=="+")
        {
            return "add ( "+left->printtree()+" , "+right->printtree()+" ) ";
        }else if(data=="-")
        {
            return "sub ("+left->printtree()+" , "+right->printtree()+" ) ";
        }else if(data=="*")
        {
            return "mul ( "+left->printtree()+" , "+right->printtree()+" ) ";
        }else if(data==">>")
        {
            return "r_shift ( "+left->printtree()+" , "+right->printtree()+" ) ";
        }else if(data=="&"||data=="|"||data=="^"||data=="<<")
        {
            return " ( " + data + " " + left->printtree()+" "+" "+right->printtree()+" ) ";
        }
        else if(data.find("pow")!=string::npos && left!=NULL)
        {
            return data+" ( "+left->printtree()+" ) ";
        }else{
            return data;
        }

    }

	string print_tree_for_cu()
	{
		if(data=="~")
		{
			return "~"+left->print_tree_for_cu();
		}else if(data=="!")
		{
			return "gfmul ( "+left->print_tree_for_cu()+" , "+right->print_tree_for_cu()+" ) ";
		}else if(data=="$")
		{
			return "sbox ( "+left->print_tree_for_cu()+" ) ";
		}else if(data=="+")
		{
			return "add ( "+left->print_tree_for_cu()+" , "+right->print_tree_for_cu()+" ) ";
		}else if(data=="-")
		{
			return "sub ("+left->print_tree_for_cu()+" , "+right->print_tree_for_cu()+" ) ";
		}else if(data=="*")
		{
			return "mul ( "+left->print_tree_for_cu()+" , "+right->print_tree_for_cu()+" ) ";
		}else if(data==">>")
		{
			return "r_shift ( "+left->print_tree_for_cu()+" , "+right->print_tree_for_cu()+" ) ";
		}else if(data=="&"||data=="|"||data=="^"||data=="<<")
		{
			return " ( "+left->print_tree_for_cu()+" "+data+" "+right->print_tree_for_cu()+" ) ";
		}
		else if(data.find("pow")!=string::npos && left!=NULL)
		{
			return data+" ( "+left->print_tree_for_cu()+" ) ";
		} else if(data.find("?")!=string::npos) {
		    string temp = data;
			return temp.replace(0, 2, "0x");
		}
		else{
			return data;
		}

	}

	void fxor(set<string> *rand,int bz=0) //change here
	{
		map<string,string>::iterator it;
		bl(this,rand);
		string exp=this->print_tree();

		//cout<<"exp: "<<exp<<endl;
		set<string>::iterator i;
		map<string,int> tmap;
		for (i=rand->begin();i!=rand->end();i++)
		{
			int num=0;
			int pos=exp.find(*i,0);
			while(pos!=exp.npos)
			{
				num++;
				pos=exp.find(*i,pos+1);
			}
			tmap.insert(pair<string,int>(*i,num));


		}

		rand->clear();
		map<string,int>::iterator mt;
		//for(mt=tmap.begin();mt!=tmap.end();mt++) cout<<mt->first<<" : "<<mt->second<<endl;
		for(mt=tmap.begin();mt!=tmap.end();mt++)
		{
			if(mt->second%2==1)
			{
				rand->insert(mt->first);
			}
		}
		//for(i=rand->begin();i!=rand->end();i++) cout<<"rand: "<<*i<<endl;
		if(bz==1)
		{
			this->data=scexp(rand);
			this->left=NULL;
			this->right=NULL;

		}


	}
	void bl(ExpTree2 *t,set<string> *rand)
	{
		if(t->data=="^")

		{
			if(t->left->data=="^")
			{
				bl(t->left,rand);
			}else{
				if(t->left->data==t->left->print_tree())
				{
					rand->insert(t->left->data);
				}else{
					rand->insert(t->left->print_tree());
					t->left->data=t->left->print_tree();
					t->left->left=NULL;
					t->left->right=NULL;
				}



			}




			if(t->right->data=="^")
			{
				bl(t->right,rand);
			}else{
				if(t->right->data==t->right->print_tree())
				{
					rand->insert(t->right->data);
				}else{
					rand->insert(t->right->print_tree());
					t->right->data=t->right->print_tree();
					t->right->left=NULL;
					t->right->right=NULL;
				}


			}


		}else if(t->data==t->print_tree()){
			rand->insert(t->data);
		}else{
			bl(t->left,rand);
			if(t->right) bl(t->right,rand);

		}
	}

	static void z(ExpTree2 *t,set<string> *rand,int q=0)
	{
		set<string> tset;
		if(t->data!=t->print_tree())
		{
			if(t->data!="^")
			{
				z(t->left,&tset,1);
				if(t->right) z(t->right,&tset,1);
				t->data=t->print_tree();
				t->left=NULL;
				t->right=NULL;
			}else{
				z(t->left,&tset,0);
				if(t->right) z(t->right,&tset,0);
				t->fxor(&tset,q);
			}
		}

		*rand=tset;

	}

    static void qcblrsa(ExpTree2 *t,set<string> *rand,set<string> *randoms)
    {
        if(t->data=="^")
        {
            if(t->left->data=="^")
            {
                qcblrsa(t->left,rand,randoms);
            }else{
                if(t->left->data==t->left->printtree())
                {
                    if((t->left->data).find("y")==(t->left->data).npos)
                    {rand->insert(t->left->data);}
                    else{rand->insert("c");

                        randoms->insert("c");
                    }
                }else{
                    string temp = t->left->printtree();
                    rand->insert(temp);
                    t->left->data=t->left->printtree();
                    t->left->left=NULL;
                    t->left->right=NULL;
                }



            }




            if(t->right->data=="^")
            {
                qcblrsa(t->right,rand,randoms);
            }else{
                if(t->right->data==t->right->printtree())
                {
                    if((t->right->data).find("y")==(t->right->data).npos)
                    {rand->insert(t->right->data);}
                    else{
                        rand->insert("c");
                        randoms->insert("c");
                    }
                }else{
                    rand->insert(t->right->printtree());
                    t->right->data=t->right->printtree();
                    t->right->left=NULL;
                    t->right->right=NULL;
                }


            }


        }
    }

	static void qcbl(ExpTree2 *t,set<string> *rand)
	{
		if(t->data=="^")
		{
			if(t->left->data=="^")
			{
				qcbl(t->left,rand);
			}else{
				if(t->left->data==t->left->printtree())
				{
					rand->insert(t->left->data);
				}else{
				    string temp = t->left->printtree();
					rand->insert(temp);
					t->left->data=t->left->printtree();
					t->left->left=NULL;
					t->left->right=NULL;
				}



			}




			if(t->right->data=="^")
			{
				qcbl(t->right,rand);
			}else{
				if(t->right->data==t->right->printtree())
				{
					rand->insert(t->right->data);
				}else{
					rand->insert(t->right->printtree());
					t->right->data=t->right->printtree();
					t->right->left=NULL;
					t->right->right=NULL;
				}


			}


		}
	}


	

	static string scexp(set<string> *tmap)
	{
		int n=0;
		string res="";
		set<string>::iterator mt;
		for(mt=tmap->begin();mt!=tmap->end();mt++)
		{
			if(n==0)
			{
				res=*mt;
			}else{
				res="( ^ "+res+" "+*mt+" )";
			}
			n++;
		}
		return res;
	}


	static list<string> hj(list<string> expr_l,set<string> randoms,set<string> keys, set<string>& new_randoms, set<string>& new_keys)
	{
		list<ExpTree2> et_l;
		list<string>::iterator sit;

		list<ExpTree2>::iterator etit;
		for(sit=expr_l.begin();sit!=expr_l.end();sit++)
		{
			list<string> s;
			s=split(*sit,s,' ');
			list<string> *ss;
			ss=&s;
			ExpTree2 f=parser(ss);
			et_l.push_back(f);


		}
		list<set<string>> l;
		list<set<string>>::iterator lt;
		list<string> res;
		list<string>::iterator resit;
		set<string> inset;
		for(etit=et_l.begin();etit!=et_l.end();etit++)
		{
			set<string> tset;
			if((*etit).data==(*etit).print_tree())
			{
				tset.insert((*etit).data);

			}else{
				z(&(*etit),&tset);

			}



			//����ż�������

			res.push_back(scexp(&tset));
		}

		list<ExpTree2> net_l;
		for(sit=res.begin();sit!=res.end();sit++)
		{
			list<string> s;
			s=split(*sit,s,' ');
			list<string> *ss;
			ss=&s;
			ExpTree2 f=parser(ss);
			net_l.push_back(f);


		}

		for(etit=net_l.begin();etit!=net_l.end();etit++)
		{
			set<string> tset;
			if(etit->data == etit->print_tree())
				tset.insert(etit->data);
			else
				qcbl(&(*etit),&tset);
				qcblrsa(&(*etit),&tset,&randoms); //  !!!!!!!!only for rsa06 !!!!!!!!!
			l.push_back(tset);

		}


		if(l.size()>1)
		{

			set<string>::iterator st;
			for(lt=l.begin();lt!=l.end();lt++)
			{
				set<string> buf;
				//cout<<"???"<<endl;
				if(lt==l.begin())
				{
					inset=*lt;
				}else{
					set_intersection((*lt).begin(),(*lt).end(),inset.begin(),inset.end(),inserter(buf,buf.begin()));
					inset=buf;
//					for(st=buf.begin();st!=buf.end();st++)
//						cout<<*st<<endl;
				}
			}

			set_intersection(inset.begin(),inset.end(),keys.begin(),keys.end(),inserter(inset,inset.begin()));
			//for(st=inset.begin();st!=inset.end();st++) cout<<"var : "<<*st<<endl;

			if(inset.size()>1)
			{
				for(lt=l.begin();lt!=l.end();lt++)
				{
					set<string> buf;
					set_difference((*lt).begin(),(*lt).end(),inset.begin(),inset.end(),inserter(buf,buf.begin()));
					*lt=buf;
					//for(st=buf.begin();st!=buf.end();st++) cout<<":::: "<<*st<<endl;
					(*lt).insert("c");
					randoms.insert("c");
				}

			}else{
                l.clear();
                for(etit=net_l.begin();etit!=net_l.end();etit++)
                {
                    set<string> tset;
                    if(etit->data == etit->print_tree())
                        tset.insert(etit->data);
                    else
                        qcbl(&(*etit),&tset);
                        qcblrsa(&(*etit),&tset,&randoms); //  !!!!!!!!only for rsa06 !!!!!!!!!
                    l.push_back(tset);

                }


			}
		}
		list<string> fin;

		for(lt=l.begin();lt!=l.end();lt++)
		{
			fin.push_back(scexp(&(*lt)));
		}



		list<set<string>> l_r;
		list<set<string>> l_k;
		list<string>::iterator fit;
		for(fit=fin.begin();fit!=fin.end();fit++)
		{
			set<string>::iterator re;
			set<string> regr;
			set<string> regk;
			for(re=randoms.begin();re!=randoms.end();re++)
			{
				int pos=(*fit).find(*re,0);
				if(pos!=(*fit).npos)
				{
					regr.insert(*re);
				}


			}
			l_r.push_back(regr);


			for(re=keys.begin();re!=keys.end();re++)
			{
				int pos=(*fit).find(*re,0);
				if(pos!=(*fit).npos)
				{
					regk.insert(*re);
				}


			}
			l_k.push_back(regk);




		}


		list<set<string>>::iterator lss;

		for(lss=l_r.begin();lss!=l_r.end();lss++)
		{
			set_union((*lss).begin(),(*lss).end(),(*(l_r.begin())).begin(),(*(l_r.begin())).end(),inserter(*(l_r.begin()),(*(l_r.begin())).begin()));


		}

		for(lss=l_k.begin();lss!=l_k.end();lss++)
		{
			set_union((*lss).begin(),(*lss).end(),(*(l_k.begin())).begin(),(*(l_k.begin())).end(),inserter(*(l_k.begin()),(*(l_k.begin())).begin()));


		}

		randoms = *(l_r.begin());
		keys =* (l_k.begin());

		new_randoms = randoms;
		new_keys = keys;


		//new!!!!!!!!
//		set<string>::iterator sst;


//		for(sst = randoms.begin();sst != randoms.end(); sst++) cout<<"f: "<<*sst<<endl;

		return fin;

	}

	static ExpTree2 parser(list<string> *s) {
        string operators="+-*&|~^>><<!$";
        stack<ExpTree2> ss;
        ExpTree2 t(""),l(""),r("");
        s->reverse();
        list<string>::iterator it;
        //cout<<"||||||||"<<endl;
        for(it=s->begin();it!=s->end();it++)
        {

            if(*it=="("||*it==")"||*it==""||*it==" "||*it=="\n"||*it=="\t")
            {
                continue;
            }else if(*it=="~")
            {
                if(ss.size()<1)
                {
                    cout<< "ExpSyntaxError0"<<endl;
                    exit(0);
                }
                t=ss.top();
                ExpTree2 expt(*it,new ExpTree2(t));
                ss.pop();
                ss.push(expt);
            }else if(*it=="$")
            {
                if(ss.size()<1)
                {
                    cout<< "ExpSyntaxError0"<<endl;
                    exit(0);
                }
                t=ss.top();
                ExpTree2 expt(*it,new ExpTree2(t));
                ss.pop();
                ss.push(expt);
            }
            else if(*it=="pow2")
            {
                if(ss.size()<1)
                {
                    cout<< "ExpSyntaxError1"<<endl;
                    exit(0);
                }
                t=ss.top();
                ExpTree2 expt(*it,new ExpTree2(t));
                ss.pop();
                ss.push(expt);
            }
			else if(*it=="sbox")
			{
				if(ss.size()<1)
				{
					cout<< "ExpSyntaxError1"<<endl;
					exit(0);
				}
				t=ss.top();
				ExpTree2 expt(*it,new ExpTree2(t));
				ss.pop();
				ss.push(expt);
			}

            else if(*it=="pow4")
            {
                if(ss.size()<1)
                {
                    cout<< "ExpSyntaxError2"<<endl;
                    exit(0);
                }
                t=ss.top();
                ExpTree2 expt(*it,new ExpTree2(t));
                ss.pop();
                ss.push(expt);
            }else if(*it=="pow8")
            {
                if(ss.size()<1)
                {
                    cout<< "ExpSyntaxError3"<<endl;
                    exit(0);
                }
                t=ss.top();
                ExpTree2 expt(*it,new ExpTree2(t));
                ss.pop();
                ss.push(expt);
            }else if(*it=="pow16")
            {
                if(ss.size()<1)
                {
                    cout<< "ExpSyntaxError4"<<endl;
                    exit(0);
                }
                t=ss.top();
                ExpTree2 expt(*it,new ExpTree2(t));
                ss.pop();
                ss.push(expt);
            }else if(operators.find(*it)!=string::npos)
            {
                if(ss.size()<2)
                {
                    cout<< "ExpSyntaxError5"<<endl;
                    exit(0);
                }
                //cout<<"<<<<<<"<<ss.size()<<endl;
                l=ss.top();
                //cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
                //cout<<"l: "<<l.data<<endl;
                ss.pop();
                //cout<<">>>>>>>"<<ss.size()<<endl;
                r=ss.top();
                //cout<<"r: "<<r.data<<endl;
                ss.pop();
                //cout<<">>>>>>>"<<ss.size()<<endl;
                //cout<<"mid: "<<it->data<<endl;
                //cout<<"l m r: "<<l.data<<it->data<<r.data<<endl;
                ExpTree2 expt=ExpTree2(*it,new ExpTree2(l),new ExpTree2(r));
                ss.push(expt);
                //cout<<">>>>>>>"<<ss.size()<<endl;
                //t=ss.top();
                //cout<<"?<><---------------------------------------><>??: "<<t.right->data<<t.data<<endl;
            }else{
                //cout<<"yyyyy: "<<it->data<<endl;
                ExpTree2 expt=ExpTree2(*it);
                ss.push(expt);
                l=ss.top();
                //cout<<"top: "<<l.data<<endl;

            }
        }
        if(ss.size()!=1)
        {

            //cout<<"???????????"<<ss.size()<<endl;
            cout<< "ExpSyntaxError6"<<endl;
            exit(0);
        }
        t=ss.top();
        //cout<<"?<><---------------------------------------><>??: "<<t.right->data<<t.data<<endl;
        return t;
	}


	static list<string> split(const string& s,list<string> l,const char flag = ' ') {
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag))
    {
        //cout<<"??"<<temp<<endl;
        l.push_back(temp);

    }
    return l;
}

	static string  cf(int num,string e)
{
	if(num==0) return "";
	string tem=e;
	for(int i=0;i<num-1;i++)
	{
		tem+=e;
	}
	e=tem;
	return e;
}


	static void  gpuFunction(list<string>& exprs, set<string>& plains, set<string>& randoms, set<string>& keys,int bit) {
        //return 1;
    	//string e1="( ^ ( $ ( ^ x x0 ) ) y ) ";
	//string e2="( ^ ( $ ( ^ x x1 ) ) y )";
	//string e3="x0";
	//list<string> exprs;
	//list<string>::iterator sit;
	//exprs.push_back(e1);
	//exprs.push_back(e2);
	//exprs.push_back(e3);

	//set<string> plains {};
	//set<string> randoms {"x0","x1","y"};
	//set<string> keys {"x"};

	int order = exprs.size();
	//string inexpr1 = "( ^ ( ^ ( $ ( ^ ( ^ ( ^ ( ^ 0x06 ( ^ ( ^ ( ^ ( ^ x x0 ) x1 ) x2 ) x3 ) ) x3 ) x2 ) x1 ) ) ( ^ ( ^ y1 y2 ) y3 ) ) y4 )";
	//string inexpr2 = "( ^ ( ^ ( $ ( ^ ( ^ ( ^ ( ^ 0x07 ( ^ ( ^ ( ^ ( ^ x x0 ) x1 ) x2 ) x3 ) ) x3 ) x2 ) x1 ) ) ( ^ ( ^ y1 y2 ) y3 ) ) y4 )";

	//list<string> expr_l;
	list<string>::iterator sit;
	string tem="";
	map<string,int> rand_var;
	list<map<string,map<string,int>> > expr_v;
	list<map<string,map<string,int>> >::iterator lmit;
	set<string>::iterator stit;
	int n=0;
	int var_num=plains.size()+randoms.size()+keys.size();
	for(sit=exprs.begin();sit!=exprs.end();sit++)
	{
		//cout<<n<<endl;
		map<string,int> tmap;
		map<string,map<string,int>> tmmap;


		for(stit=plains.begin();stit!=plains.end();stit++)
		{
			int num=0;
			int pos=sit->find(*stit,0);
			while(pos!=sit->npos)
			{
				num++;
				pos=sit->find(*stit,pos+1);
			}
			tmap.insert(pair<string,int>(*stit,num));
		}
		tmmap.insert(pair<string,map<string,int>>("plains",tmap));
		expr_v.push_back(tmmap);
		tmap.erase(tmap.begin(),tmap.end());

		for(stit=randoms.begin();stit!=randoms.end();stit++)
		{
			int num=0;
			int pos=sit->find(*stit,0);
			while(pos!=sit->npos)
			{
				num++;
				pos=sit->find(*stit,pos+1);
			}
			map<string,int>::iterator reg1;
			if(rand_var.find(*stit)!=rand_var.end())
			{
				rand_var[*stit]=rand_var[*stit]+num;
			}else{
				rand_var[*stit]=num;
			}
			tmap.insert(pair<string,int>(*stit,num));
		}
		tmmap.insert(pair<string,map<string,int>>("randoms",tmap));
		expr_v.push_back(tmmap);

		tmap.erase(tmap.begin(),tmap.end());

		for(stit=keys.begin();stit!=keys.end();stit++)
		{
			int num=0;
			int pos=sit->find(*stit,0);
			while(pos!=sit->npos)
			{
				num++;
				pos=sit->find(*stit,pos+1);
			}
			tmap.insert(pair<string,int>(*stit,num));
		}
		tmmap.insert(pair<string,map<string,int>>("keys",tmap));
		expr_v.push_back(tmmap);

		tmap.erase(tmap.begin(),tmap.end());

		n++;
	}
	list<ExpTree2> et_l;
	list<ExpTree2>::iterator etit;
	for(sit=exprs.begin();sit!=exprs.end();sit++)
	{
		list<string> s;
		s=split(*sit,s,' ');
		list<string> *ss;
		ss=&s;
		ExpTree2 f=parser(ss);
		et_l.push_back(f);


	}
	/*
	for(etit=et_l.begin();etit!=et_l.end();etit++)
	{
		string expr=etit->print_tree();
		cout<<"expr: "<<expr<<endl;
	}
	*/
	/*


	������ʽ


	*/

	/*
	list<string> s1;
	list<string> s2;
	list<string>::iterator it;
	s1=split(inexpr1,s1,' ');
	s2=split(inexpr2,s2,' ');
	list<string> *ss1,*ss2;
	ss1=&s1;
	ss2=&s2;

	//int rand_num=8;
	//int key_num=1;

	ExpTree2 f1=parser(ss1);
	string expr1=f1.print_tree();
	ExpTree2 f2=parser(ss2);
	string expr2=f2.print_tree();
	cout<<expr1<<endl;
	cout<<expr2<<endl;
	*/


	string elem="";
	string ker_elem="";
	string ker="";

	for(stit=randoms.begin();stit!=randoms.end();stit++)
	{
		elem+="int "+*stit+",";
		//cout<<"????: "<<*stit<<endl;
	}
	int kn=0;
	for(stit=keys.begin();stit!=keys.end();stit++)
	{
		elem+="int "+*stit+",";
		string reg=cf(kn+1,"k");
		ker_elem+="int "+reg+",";
		ker+=reg+",";
		kn++;
	}
	int pn=0;
	for(stit=plains.begin();stit!=plains.end();stit++)
	{
		elem+="int "+*stit+",";
		string reg=cf(pn+1,"p");
		ker_elem+="int "+reg+",";
		ker+=reg+",";
		pn++;
	}

		if(bit==8)
		{
			if(randoms.size()<3)
			{
				for(int i=randoms.size();i<3;i++)
				{
					string reg0=cf(i,"z");
					elem="int "+reg0+","+elem;
				}
			}
		}

		elem.pop_back();
	ker_elem.pop_back();
	ker.pop_back();
	//cout<<elem<<endl;
	
	//cout<<"2222222222222222222"<<endl;
	ofstream out;
	out.open("ct.cu",ios::trunc); //ios::trunc��ʾ�ڴ��ļ�ǰ���ļ����,������д��,�ļ��������򴴽�
	out<<"#include <stdio.h>\n#include<iostream>\n#include <cuda_runtime.h>\n#include<time.h>\n#include<map>\nusing namespace std;"<<"\n";
	//sbox
	
	//out<<elem;
	
	
	int sint=pow(2,bit);
	string s;
	stringstream ss;
	ss<<sint;
	ss>>s;
	
	
	if (bit==8)
	{
		out<<"extern __constant__ int s[256] = {\n0x63,  0x7c,  0x77,  0x7b,  0xf2,  0x6b,  0x6f,  0xc5,  0x30,  0x01,  0x67,  0x2b,  0xfe,  0xd7,  0xab,  0x76,  0xca,  0x82,  0xc9,  0x7d,  0xfa,  0x59,  0x47,  0xf0,  0xad,  0xd4,  0xa2,  0xaf,  0x9c,  0xa4,  0x72,  0xc0,  0xb7,  0xfd,  0x93,  0x26,  0x36,  0x3f,  0xf7,  0xcc,  0x34,  0xa5,  0xe5,  0xf1,  0x71,  0xd8,  0x31,  0x15,  0x04,  0xc7,  0x23,  0xc3,  0x18,  0x96,  0x05,  0x9a,  0x07,  0x12,  0x80,  0xe2,  0xeb,  0x27,  0xb2,  0x75,  0x09,  0x83,  0x2c,  0x1a,  0x1b,  0x6e,  0x5a,  0xa0,  0x52,  0x3b,  0xd6,  0xb3,  0x29,  0xe3,  0x2f,  0x84,  0x53,  0xd1,  0x00,  0xed,  0x20,  0xfc,  0xb1,  0x5b,  0x6a,  0xcb,  0xbe,  0x39,  0x4a,  0x4c,  0x58,  0xcf,  0xd0,  0xef,  0xaa,  0xfb,  0x43,  0x4d,  0x33,  0x85,  0x45,  0xf9,  0x02,  0x7f,  0x50,  0x3c,  0x9f,  0xa8,  0x51,  0xa3,  0x40,  0x8f,  0x92,  0x9d,  0x38,  0xf5,  0xbc,  0xb6,  0xda,  0x21,  0x10,  0xff,  0xf3,  0xd2,  0xcd,  0x0c,  0x13,  0xec,  0x5f,  0x97,  0x44,  0x17,  0xc4,  0xa7,  0x7e,  0x3d,  0x64,  0x5d,  0x19,  0x73,  0x60,  0x81,  0x4f,  0xdc,  0x22,  0x2a,  0x90,  0x88,  0x46,  0xee,  0xb8,  0x14,  0xde,  0x5e,  0x0b,  0xdb,  0xe0,  0x32,  0x3a,  0x0a,  0x49,  0x06,  0x24,  0x5c,  0xc2,  0xd3,  0xac,  0x62,  0x91,  0x95,  0xe4,  0x79,  0xe7,  0xc8,  0x37,  0x6d,  0x8d,  0xd5,  0x4e,  0xa9,  0x6c,  0x56,  0xf4,  0xea,  0x65,  0x7a,  0xae,  0x08,  0xba,  0x78,  0x25,  0x2e,  0x1c,  0xa6,  0xb4,  0xc6,  0xe8,  0xdd,  0x74,  0x1f,  0x4b,  0xbd,  0x8b,  0x8a,  0x70,  0x3e,  0xb5,  0x66,  0x48,  0x03,  0xf6,  0x0e,  0x61,  0x35,  0x57,  0xb9,  0x86,  0xc1,  0x1d,  0x9e,  0xe1,  0xf8,  0x98,  0x11,  0x69,  0xd9,  0x8e,  0x94,  0x9b,  0x1e,  0x87,  0xe9,  0xce,  0x55,  0x28,  0xdf,  0x8c,  0xa1,  0x89,  0x0d,  0xbf,  0xe6,  0x42,  0x68,  0x41,  0x99,  0x2d,  0x0f,  0xb0,  0x54,  0xbb,  0x16\n};\n";


		out<<"extern __constant__ int table[798] = {\n 0x00, 0x00, 0x19, 0x01, 0x32, 0x02, 0x1a, 0xc6, 0x4b, 0xc7, 0x1b, 0x68, 0x33, 0xee, 0xdf, 0x03,\n 0x64, 0x04, 0xe0, 0x0e, 0x34, 0x8d, 0x81, 0xef, 0x4c, 0x71, 0x08, 0xc8, 0xf8, 0x69, 0x1c, 0xc1,\n 0x7d, 0xc2, 0x1d, 0xb5, 0xf9, 0xb9, 0x27, 0x6a, 0x4d, 0xe4, 0xa6, 0x72, 0x9a, 0xc9, 0x09, 0x78,\n 0x65, 0x2f, 0x8a, 0x05, 0x21, 0x0f, 0xe1, 0x24, 0x12, 0xf0, 0x82, 0x45, 0x35, 0x93, 0xda, 0x8e,\n 0x96, 0x8f, 0xdb, 0xbd, 0x36, 0xd0, 0xce, 0x94, 0x13, 0x5c, 0xd2, 0xf1, 0x40, 0x46, 0x83, 0x38,\n 0x66, 0xdd, 0xfd, 0x30, 0xbf, 0x06, 0x8b, 0x62, 0xb3, 0x25, 0xe2, 0x98, 0x22, 0x88, 0x91, 0x10,\n 0x7e, 0x6e, 0x48, 0xc3, 0xa3, 0xb6, 0x1e, 0x42, 0x3a, 0x6b, 0x28, 0x54, 0xfa, 0x85, 0x3d, 0xba,\n 0x2b, 0x79, 0x0a, 0x15, 0x9b, 0x9f, 0x5e, 0xca, 0x4e, 0xd4, 0xac, 0xe5, 0xf3, 0x73, 0xa7, 0x57,\n 0xaf, 0x58, 0xa8, 0x50, 0xf4, 0xea, 0xd6, 0x74, 0x4f, 0xae, 0xe9, 0xd5, 0xe7, 0xe6, 0xad, 0xe8,\n 0x2c, 0xd7, 0x75, 0x7a, 0xeb, 0x16, 0x0b, 0xf5, 0x59, 0xcb, 0x5f, 0xb0, 0x9c, 0xa9, 0x51, 0xa0,\n 0x7f, 0x0c, 0xf6, 0x6f, 0x17, 0xc4, 0x49, 0xec, 0xd8, 0x43, 0x1f, 0x2d, 0xa4, 0x76, 0x7b, 0xb7,\n 0xcc, 0xbb, 0x3e, 0x5a, 0xfb, 0x60, 0xb1, 0x86, 0x3b, 0x52, 0xa1, 0x6c, 0xaa, 0x55, 0x29, 0x9d,\n 0x97, 0xb2, 0x87, 0x90, 0x61, 0xbe, 0xdc, 0xfc, 0xbc, 0x95, 0xcf, 0xcd, 0x37, 0x3f, 0x5b, 0xd1,\n 0x53, 0x39, 0x84, 0x3c, 0x41, 0xa2, 0x6d, 0x47, 0x14, 0x2a, 0x9e, 0x5d, 0x56, 0xf2, 0xd3, 0xab,\n 0x44, 0x11, 0x92, 0xd9, 0x23, 0x20, 0x2e, 0x89, 0xb4, 0x7c, 0xb8, 0x26, 0x77, 0x99, 0xe3, 0xa5,\n 0x67, 0x4a, 0xed, 0xde, 0xc5, 0x31, 0xfe, 0x18, 0x0d, 0x63, 0x8c, 0x80, 0xc0, 0xf7, 0x70, 0x07,\n\n\n 0x01, 0x03, 0x05, 0x0f, 0x11, 0x33, 0x55, 0xff, 0x1a, 0x2e, 0x72, 0x96, 0xa1, 0xf8, 0x13, 0x35,\n 0x5f, 0xe1, 0x38, 0x48, 0xd8, 0x73, 0x95, 0xa4, 0xf7, 0x02, 0x06, 0x0a, 0x1e, 0x22, 0x66, 0xaa,\n 0xe5, 0x34, 0x5c, 0xe4, 0x37, 0x59, 0xeb, 0x26, 0x6a, 0xbe, 0xd9, 0x70, 0x90, 0xab, 0xe6, 0x31,\n 0x53, 0xf5, 0x04, 0x0c, 0x14, 0x3c, 0x44, 0xcc, 0x4f, 0xd1, 0x68, 0xb8, 0xd3, 0x6e, 0xb2, 0xcd,\n 0x4c, 0xd4, 0x67, 0xa9, 0xe0, 0x3b, 0x4d, 0xd7, 0x62, 0xa6, 0xf1, 0x08, 0x18, 0x28, 0x78, 0x88,\n 0x83, 0x9e, 0xb9, 0xd0, 0x6b, 0xbd, 0xdc, 0x7f, 0x81, 0x98, 0xb3, 0xce, 0x49, 0xdb, 0x76, 0x9a,\n 0xb5, 0xc4, 0x57, 0xf9, 0x10, 0x30, 0x50, 0xf0, 0x0b, 0x1d, 0x27, 0x69, 0xbb, 0xd6, 0x61, 0xa3,\n 0xfe, 0x19, 0x2b, 0x7d, 0x87, 0x92, 0xad, 0xec, 0x2f, 0x71, 0x93, 0xae, 0xe9, 0x20, 0x60, 0xa0,\n 0xfb, 0x16, 0x3a, 0x4e, 0xd2, 0x6d, 0xb7, 0xc2, 0x5d, 0xe7, 0x32, 0x56, 0xfa, 0x15, 0x3f, 0x41,\n 0xc3, 0x5e, 0xe2, 0x3d, 0x47, 0xc9, 0x40, 0xc0, 0x5b, 0xed, 0x2c, 0x74, 0x9c, 0xbf, 0xda, 0x75,\n 0x9f, 0xba, 0xd5, 0x64, 0xac, 0xef, 0x2a, 0x7e, 0x82, 0x9d, 0xbc, 0xdf, 0x7a, 0x8e, 0x89, 0x80,\n 0x9b, 0xb6, 0xc1, 0x58, 0xe8, 0x23, 0x65, 0xaf, 0xea, 0x25, 0x6f, 0xb1, 0xc8, 0x43, 0xc5, 0x54,\n 0xfc, 0x1f, 0x21, 0x63, 0xa5, 0xf4, 0x07, 0x09, 0x1b, 0x2d, 0x77, 0x99, 0xb0, 0xcb, 0x46, 0xca,\n 0x45, 0xcf, 0x4a, 0xde, 0x79, 0x8b, 0x86, 0x91, 0xa8, 0xe3, 0x3e, 0x42, 0xc6, 0x51, 0xf3, 0x0e,\n 0x12, 0x36, 0x5a, 0xee, 0x29, 0x7b, 0x8d, 0x8c, 0x8f, 0x8a, 0x85, 0x94, 0xa7, 0xf2, 0x0d, 0x17,\n 0x39, 0x4b, 0xdd, 0x7c, 0x84, 0x97, 0xa2, 0xfd, 0x1c, 0x24, 0x6c, 0xb4, 0xc7, 0x52, 0xf6, 0x01,\n\n\n 0x03, 0x05, 0x0f, 0x11, 0x33, 0x55, 0xff, 0x1a, 0x2e, 0x72, 0x96, 0xa1, 0xf8, 0x13, 0x35,\n 0x5f, 0xe1, 0x38, 0x48, 0xd8, 0x73, 0x95, 0xa4, 0xf7, 0x02, 0x06, 0x0a, 0x1e, 0x22, 0x66, 0xaa,\n 0xe5, 0x34, 0x5c, 0xe4, 0x37, 0x59, 0xeb, 0x26, 0x6a, 0xbe, 0xd9, 0x70, 0x90, 0xab, 0xe6, 0x31,\n 0x53, 0xf5, 0x04, 0x0c, 0x14, 0x3c, 0x44, 0xcc, 0x4f, 0xd1, 0x68, 0xb8, 0xd3, 0x6e, 0xb2, 0xcd,\n 0x4c, 0xd4, 0x67, 0xa9, 0xe0, 0x3b, 0x4d, 0xd7, 0x62, 0xa6, 0xf1, 0x08, 0x18, 0x28, 0x78, 0x88,\n 0x83, 0x9e, 0xb9, 0xd0, 0x6b, 0xbd, 0xdc, 0x7f, 0x81, 0x98, 0xb3, 0xce, 0x49, 0xdb, 0x76, 0x9a,\n 0xb5, 0xc4, 0x57, 0xf9, 0x10, 0x30, 0x50, 0xf0, 0x0b, 0x1d, 0x27, 0x69, 0xbb, 0xd6, 0x61, 0xa3,\n 0xfe, 0x19, 0x2b, 0x7d, 0x87, 0x92, 0xad, 0xec, 0x2f, 0x71, 0x93, 0xae, 0xe9, 0x20, 0x60, 0xa0,\n 0xfb, 0x16, 0x3a, 0x4e, 0xd2, 0x6d, 0xb7, 0xc2, 0x5d, 0xe7, 0x32, 0x56, 0xfa, 0x15, 0x3f, 0x41,\n 0xc3, 0x5e, 0xe2, 0x3d, 0x47, 0xc9, 0x40, 0xc0, 0x5b, 0xed, 0x2c, 0x74, 0x9c, 0xbf, 0xda, 0x75,\n 0x9f, 0xba, 0xd5, 0x64, 0xac, 0xef, 0x2a, 0x7e, 0x82, 0x9d, 0xbc, 0xdf, 0x7a, 0x8e, 0x89, 0x80,\n 0x9b, 0xb6, 0xc1, 0x58, 0xe8, 0x23, 0x65, 0xaf, 0xea, 0x25, 0x6f, 0xb1, 0xc8, 0x43, 0xc5, 0x54,\n 0xfc, 0x1f, 0x21, 0x63, 0xa5, 0xf4, 0x07, 0x09, 0x1b, 0x2d, 0x77, 0x99, 0xb0, 0xcb, 0x46, 0xca,\n 0x45, 0xcf, 0x4a, 0xde, 0x79, 0x8b, 0x86, 0x91, 0xa8, 0xe3, 0x3e, 0x42, 0xc6, 0x51, 0xf3, 0x0e,\n 0x12, 0x36, 0x5a, 0xee, 0x29, 0x7b, 0x8d, 0x8c, 0x8f, 0x8a, 0x85, 0x94, 0xa7, 0xf2, 0x0d, 0x17,\n 0x39, 0x4b, 0xdd, 0x7c, 0x84, 0x97, 0xa2, 0xfd, 0x1c, 0x24, 0x6c, 0xb4, 0xc7, 0x52, 0xf6, 0x01\n};\n";

		out<<"__device__ int sbox(int a) {\n	{return s[(a/16)*16+a%16];}\n}\n";

		out<<"__device__ int gfmul(int a, int b) {\n	if(a == 0 || b == 0)\n	{return 0;}\n	else\n	{return table[table[a] + table[b] + 256];}\n}\n";

		out<<"__device__ int pow2(int a) {\n	return gfmul(a,a);\n}\n";

		out<<"__device__ int pow4(int a) {\n	return pow2(pow2(a));\n}\n";

		out<<"__device__ int pow8(int a) {\n	return pow4(pow2(a));\n}\n";

		out<<"__device__ int pow16(int a) {\n	return pow8(pow2(a));\n}\n";

		out<<"__device__ int add(int a,int b)\n{\n	if((a + b)<0)\n	{\n		return ((a + b) % 256)+256;\n	}else{\n		return (a + b) % 256;\n	}\n}\n";

		out<<"__device__ int sub(int a,int b)\n{\n	if((a - b)<0)\n	{\n		return ((a - b) % 256)+256;\n	}else{\n		return (a - b) % 256;\n	}\n}\n";

		out<<"__device__ int mul(int a,int b)\n{\n	if((a * b)<0)\n	{\n		return ((a * b) % 256)+256;\n	}else{\n		return (a * b) % 256;\n	}\n}\n";

		out<<"__device__ int r_shift(int a,int b)\n{\n	if((a >> b)<0)\n	{\n		return ((a >> b) % 256)+256;\n	}else{\n		return (a >> b) % 256;\n	}\n}\n";

	
	
	}
	
	
	//���ʽ��ֵ

	int index=0;
	if(bit==8)
	{
		for(etit=et_l.begin();etit!=et_l.end();etit++)
		{
			out<<"__device__ int getExpValue"+to_string(index)+"("+elem+") {\n	return "+etit->print_tree_for_cu()+";\n}\n";
			index++;
		}
		
	}else{
		for(etit=et_l.begin();etit!=et_l.end();etit++)
		{
			out<<"__device__ bool getExpValue"+to_string(index)+"("+elem+") {\n	return "+etit->print_tree_for_cu()+";\n}\n";
			index++;
		}
	}
	

	//kernel function
	//cout<<"333333333333333333333"<<endl;
	out<<"__global__ void expKernel(int* A,"+ker_elem+") {\n";
	
	
	if(bit==8)
	{
		int fnum=0;
		string in_ker="";
		int tempSize = randoms.size() - 3;
		for(int i=0;i<tempSize; i++)
		{
			string reg2=cf(i+1,"i");
			in_ker+=(reg2+",");
			out<<"	for(int "+ reg2 +"=0;"+reg2+"<256;"+reg2+"++){\n";
			fnum++;
		}





		string A_index="";
		for(int i=0;i<order;i++)
		{
			string reg3=cf(i,"*256");
			A_index+="getExpValue"+to_string(i)+"("+in_ker+"threadIdx.y * 16 + threadIdx.x,blockIdx.x,blockIdx.y,"+ker+")"+reg3+"+";
		}
		A_index.pop_back();

		out<<"	atomicAdd(&A["+A_index+"],1);\n";

		for(int i=0;i<fnum;i++)
		{

			out<<"	}\n";
		}	
	}else{
		int fnum=0;
		string in_ker="";
		int tempSize = randoms.size() - 25;
		//cout<<"???: "<<tempSize<<endl;
		for(int i=0;i<tempSize; i++)
		{
			string reg2=cf(i+1,"i");
			in_ker+=(reg2+",");
			out<<"	for(int "+ reg2 +"=0;"+reg2+"<2;"+reg2+"++){\n";
			fnum++;
		}

		
		string idbx="";
		int syr=randoms.size();
		syr=syr-10;
		if(syr>=0)
		{
			idbx="threadIdx.x%2, (threadIdx.x/2)%2,(threadIdx.x/4)%2,(threadIdx.x/8)%2,(threadIdx.x/16)%2,(threadIdx.x/32)%2, (threadIdx.x/64)%2,(threadIdx.x/128)%2,(threadIdx.x/256)%2,(threadIdx.x/512)%2,";
		}else{
			for(int i=0;i<randoms.size();i++)
			{
				string buf;
				stringstream ljt;
				ljt<<pow(2,i);
				ljt>>buf;
				idbx+="(threadIdx.x/"+buf+")%2,";
			}
			
		}
		//cout<<"5555555555555555555"<<endl;
		string idgx="";
		if(randoms.size()<26)
		{
			
			
			for(int i=0;i<syr;i++)
			{
				//cout<<"777777777777777: "<<syr<<endl;
				string buf;
				stringstream ljt;
				ljt<<pow(2,i);
				ljt>>buf;
				idgx+="(blockIdx.x/"+buf+")%2,";
			}
			
		}
		//cout<<"6666666666666666"<<endl;

		string A_index="";
		for(int i=0;i<order;i++)
		{
			string reg3=cf(i,"*2");
			A_index+="getExpValue"+to_string(i)+"("+in_ker+idbx+idgx+ker+")"+reg3+"+";
		}
		A_index.pop_back();

		out<<"	atomicAdd(&A["+A_index+"],1);\n";

		for(int i=0;i<fnum;i++)
		{

			out<<"	}\n";
		}
		
	
	}
	//cout<<"444444444444444444444"<<endl;
	

	out<<"\n}\n";



	out<<"int main() {\n	clock_t s,f;\n	s=clock();\n	int N=pow("+s+","+to_string(order)+");\n	size_t size = N*sizeof(int);\n	int* d_A;\n	int* d_B;\n	bool SID=1;\n	";
	out<<"\n	cudaMallocManaged(&d_A, size);\n	cudaMallocManaged(&d_B, size);\n	for(int a=0;a<N;a++) d_A[a]=0;\n";
	
	if(bit==8)
	{
		out<<"	dim3 block(16,16);\n	dim3 grid(4096/block.x,4096/block.y);\n";
	}else{
		int tempi=randoms.size()-10;
		if(tempi<=0)
		{
			tempi=1;
		}else{
			tempi=pow(2,tempi);
		}
		
		string gridy;
		stringstream tempiss;
		tempiss<<tempi;
		tempiss>>gridy;
		
		out<<"	dim3 block(1024,1);\n	dim3 grid("+gridy+",1);\n";
	}
	
	


	string reg4=cf(plains.size()+keys.size(),"0,");
	reg4.pop_back();



	out<<"	expKernel<<<grid, block>>>(d_A,"+reg4+");\n	cudaDeviceSynchronize();\n";


	int fnum=0;
	string in_for="";
	for(int i=0;i<plains.size()+keys.size();i++)
	{
		string reg2=cf(i+1,"i");
		in_for+=(reg2+",");
		out<<"	for(int "+ reg2 +"=0;"+reg2+"<"+s+";"+reg2+"++){\n";
		fnum++;
	}
	in_for.pop_back();

	out<<"		for(int b=0;b<N;b++) d_B[b]=0;\n"<<endl;
	out<<"		expKernel<<<grid, block>>>(d_B,"+in_for+");\n		cudaDeviceSynchronize();\n		int count=0;\n		while(SID && count < N)\n		{\n			if(d_B[count]!=d_A[count])\n			{\n				SID=0;\n			}\n			count++;\n		}\n		if(SID==0)\n		{\n			cudaFree(d_A);\n			cudaFree(d_B);\n			cout<<\"0\"<<endl;\n f=clock();\n	cout<<(double)(f-s)/CLOCKS_PER_SEC<<endl;\n return 0;\n		}\n";
	for(int i=0;i<fnum;i++)
	{

		out<<"	}\n";
	}

	out<<"	cudaFree(d_A);\n	cudaFree(d_B);\n	cout<<\"1\"<<endl;\n f=clock();\n	cout<<(double)(f-s)/CLOCKS_PER_SEC<<endl;\n	\n}";
	//out<<"~~~~~~~~~~~~~~~~~~~~~~~~";
	out.close();
	//cout<<"1"<<endl;
}



	static void ExploitRandoms(vector<shared_ptr<ExpTree2>>& f, int index, vector<string>& randoms, map<string,int>& context, vector<int>& dist)
	{
		string rr,v;
		int vv;

		list<string>::iterator it,itt,it1;

		rr = randoms[index];

		if(index==0)
		{
			for(int i=0; i < domain; i++)
			{
				context[rr]=i;

				int len = f.size();
				int res = 0;
				for(int i = 0; i < f.size(); i++) {
				    int value = f[i]->evaluate(context);
				    res += value * pow(domain, i);
				}

				dist[res]++;
			}
			context[rr]=0;
			return;
		}else{
			for(int i=1; i < domain ;i++)
			{
				context[rr]=i;

				ExploitRandoms(f,index-1, randoms,context, dist);
			}
			context[rr]=0;

		}
		return;



	}

	static void count(vector<shared_ptr<ExpTree2>>& f, vector<string>& randoms, map<string,int>& context, vector<int>& dist)
	{
		if(randoms.size() == 0)
		{
		    int len = f.size();
            int res = 0;
            for(int i = 0; i < f.size(); i++) {
                int value = f[i]->evaluate(context);
                res += value * pow(domain, i);
            }

            dist[res]++;

		} else {
			for(int i=0; i < randoms.size(); i++)
			{
				ExploitRandoms(f, i, randoms, context, dist);
			}
		}
	}

	static bool ExploitKey(vector<shared_ptr<ExpTree2>>& f, int index, vector<string>& keys, vector<string>& randoms, map<string,int>& context,  vector<int>& dist1, vector<int>& dist2)
	{
		string kk = keys[index];

		if(index == 0) {
			for(int i = 0; i < domain; i++) {
				context[kk]=i;

				if(isArrayEmpty(dist1)) {
					count(f, randoms, context, dist1);
				} else {
					count(f, randoms, context, dist2);

					if(compareTwoArray(dist1, dist2) == false)
					{
						return false;
					}

					for(int i = 0; i < dist2.size(); i++)
					    dist2[i] = 0;
				}
			}
			context[kk]=0;

		}else {
			for(int i = 1; i < domain; i++) {
				context[kk]=i;

				bool result = ExploitKey(f,index-1,keys,randoms,context,dist1, dist2);

				if(not result) {
					return false;
				}
			}
			context[kk]=0;

		}
		return true;

	}


        static bool compareTwoArray(vector<int>& dist1, vector<int>& dist2) {
			for(int i = 0; i < dist1.size(); i++) {
                if (dist1[i] == dist2[i])
                    continue;
                else
                    return false;
            }
            return true;
		}

	static double ExploitKeys(vector<shared_ptr<ExpTree2>>& f, vector<string>& keys, vector<string>& randoms, map<string, int>& context, vector<int>& dist1, vector<int>& dist2)
	{
		for(int i=0; i < (int)keys.size(); i++)
		{
			double result = ExploitKey(f,i,keys,randoms,context, dist1, dist2);
			if(not result)
				return 0;
		}
		return 1;

	}


	/**
	static string cpuFunction(list<string>& exprs, set<string>& plains, set<string>& randoms, set<string>& keys,int bit) {

			vector<ExpTree2Ptr> f;
			for(string exp : exprs) {
				list<string> s;
				s = split(exp, s, ' ');
				list<string> *ss;
				ss = &s;
				ExpTree2Ptr et = parser(ss);
				f.push_back(et);
			}
			cout << "size: " << f.size() << endl;

			vector<string> plainsV(plains.begin(), plains.end());
			vector<string> randomsV(randoms.begin(), randoms.end());
			vector<string> keysV(keys.begin(), keys.end());

			return checkSID(f, plainsV, keysV, randomsV);


		}
	 **/
	static string checkSID(vector<shared_ptr<ExpTree2>>& f, vector<string>& plains, vector<string>& keys, vector<string>& randoms) {
		stringstream stream;
		map<string, int> context;
		int length = pow(domain, f.size()) - 1;
        vector<int> dist1(length, 0);
        vector<int> dist2(length, 0);

		for(string n : plains)
			context[n] = 0;

		for(string n : keys)
			context[n] = 0;

		for(string n : randoms)
			context[n] = 0;

		// if no plains
		if(plains.size() == 0) {

			double result = ExploitKeys(f, keys, randoms, context, dist1, dist2);

			if(not result) {
				return "SDD";
			} else{
				return "SID";
			}
		}else{
			for(int i=0; i < plains.size();i++)
			{
				//double result=ExploitPlain(f,i,plains,keys,randoms,context, &dist1, &dist2);
				bool result = 1;
				assert(false);
				if(not result)
				{
					return "SDD";
				}
				else return "SID";

			}
		}
		return "SDD";
	}

	int evaluate(map<string,int>& context)
	{
		stringstream stream;
		//cout<<"WWWWWWWWWWWWWWWWWWWWWW"<<data<<endl;
		if(data=="+")
		{
			return (left->evaluate(context) + right->evaluate(context)) % domain;
		}else if(data=="-")
		{
			return (left->evaluate(context) - right->evaluate(context))% domain;
		}else if(data=="*")
		{
			return (left->evaluate(context) * right->evaluate(context)) % domain;
		}else if(data=="&")
		{
			return left->evaluate(context) & right->evaluate(context);
		}else if(data=="|")
		{
			return left->evaluate(context) | right->evaluate(context);
		}else if(data=="~")
		{
			return (~left->evaluate(context))% domain;

		}else if(data=="^")
		{
			return left->evaluate(context) ^ right->evaluate(context);
		}else if(data==">>")
		{
			return (left->evaluate(context) >>right->evaluate(context))% domain;
		}else if(data=="<<")
		{
			return (left->evaluate(context) << right->evaluate(context))% domain;

		}else if(data=="!")
		{
			return GFM(left->evaluate(context), right->evaluate(context))% domain;
		}else{

			int d;

			if(isNumber(data))
			{
				stream<<data;
				stream>>d;
				//cout<<"GGGGGGGGGGGGGGGGGGGG<"<<data<<endl;
				if (d>domain)
				{
					cout<< "ExpSyntaxError"<<endl;
					exit(0);
				}else{
					return d;
				}
			}else{
				return context[data];
			}


		}

	}

	static int GFM(int x,int y){
		set<int> table={0x00, 0x00, 0x19, 0x01, 0x32, 0x02, 0x1a, 0xc6, 0x4b, 0xc7, 0x1b, 0x68, 0x33, 0xee, 0xdf, 0x03, 0x64, 0x04, 0xe0, 0x0e, 0x34, 0x8d, 0x81, 0xef, 0x4c, 0x71, 0x08, 0xc8, 0xf8, 0x69, 0x1c, 0xc1, 0x7d, 0xc2, 0x1d, 0xb5, 0xf9, 0xb9, 0x27, 0x6a, 0x4d, 0xe4, 0xa6, 0x72, 0x9a, 0xc9, 0x09, 0x78,  0x65, 0x2f, 0x8a, 0x05, 0x21, 0x0f, 0xe1, 0x24, 0x12, 0xf0, 0x82, 0x45, 0x35, 0x93, 0xda, 0x8e, 0x96, 0x8f, 0xdb, 0xbd, 0x36, 0xd0, 0xce, 0x94, 0x13, 0x5c, 0xd2, 0xf1, 0x40, 0x46, 0x83, 0x38,     0x66, 0xdd, 0xfd, 0x30, 0xbf, 0x06, 0x8b, 0x62, 0xb3, 0x25, 0xe2, 0x98, 0x22, 0x88, 0x91, 0x10,  0x7e, 0x6e, 0x48, 0xc3, 0xa3, 0xb6, 0x1e, 0x42, 0x3a, 0x6b, 0x28, 0x54, 0xfa, 0x85, 0x3d, 0xba,     0x2b, 0x79, 0x0a, 0x15, 0x9b, 0x9f, 0x5e, 0xca, 0x4e, 0xd4, 0xac, 0xe5, 0xf3, 0x73, 0xa7, 0x57,     0xaf, 0x58, 0xa8, 0x50, 0xf4, 0xea, 0xd6, 0x74, 0x4f, 0xae, 0xe9, 0xd5, 0xe7, 0xe6, 0xad, 0xe8,     0x2c, 0xd7, 0x75, 0x7a, 0xeb, 0x16, 0x0b, 0xf5, 0x59, 0xcb, 0x5f, 0xb0, 0x9c, 0xa9, 0x51, 0xa0,     0x7f, 0x0c, 0xf6, 0x6f, 0x17, 0xc4, 0x49, 0xec, 0xd8, 0x43, 0x1f, 0x2d, 0xa4, 0x76, 0x7b, 0xb7,     0xcc, 0xbb, 0x3e, 0x5a, 0xfb, 0x60, 0xb1, 0x86, 0x3b, 0x52, 0xa1, 0x6c, 0xaa, 0x55, 0x29, 0x9d,     0x97, 0xb2, 0x87, 0x90, 0x61, 0xbe, 0xdc, 0xfc, 0xbc, 0x95, 0xcf, 0xcd, 0x37, 0x3f, 0x5b, 0xd1,     0x53, 0x39, 0x84, 0x3c, 0x41, 0xa2, 0x6d, 0x47, 0x14, 0x2a, 0x9e, 0x5d, 0x56, 0xf2, 0xd3, 0xab,     0x44, 0x11, 0x92, 0xd9, 0x23, 0x20, 0x2e, 0x89, 0xb4, 0x7c, 0xb8, 0x26, 0x77, 0x99, 0xe3, 0xa5,     0x67, 0x4a, 0xed, 0xde, 0xc5, 0x31, 0xfe, 0x18, 0x0d, 0x63, 0x8c, 0x80, 0xc0, 0xf7, 0x70, 0x07,    0x01, 0x03, 0x05, 0x0f, 0x11, 0x33, 0x55, 0xff, 0x1a, 0x2e, 0x72, 0x96, 0xa1, 0xf8, 0x13, 0x35,     0x5f, 0xe1, 0x38, 0x48, 0xd8, 0x73, 0x95, 0xa4, 0xf7, 0x02, 0x06, 0x0a, 0x1e, 0x22, 0x66, 0xaa,     0xe5, 0x34, 0x5c, 0xe4, 0x37, 0x59, 0xeb, 0x26, 0x6a, 0xbe, 0xd9, 0x70, 0x90, 0xab, 0xe6, 0x31,     0x53, 0xf5, 0x04, 0x0c, 0x14, 0x3c, 0x44, 0xcc, 0x4f, 0xd1, 0x68, 0xb8, 0xd3, 0x6e, 0xb2, 0xcd,     0x4c, 0xd4, 0x67, 0xa9, 0xe0, 0x3b, 0x4d, 0xd7, 0x62, 0xa6, 0xf1, 0x08, 0x18, 0x28, 0x78, 0x88,     0x83, 0x9e, 0xb9, 0xd0, 0x6b, 0xbd, 0xdc, 0x7f, 0x81, 0x98, 0xb3, 0xce, 0x49, 0xdb, 0x76, 0x9a,     0xb5, 0xc4, 0x57, 0xf9, 0x10, 0x30, 0x50, 0xf0, 0x0b, 0x1d, 0x27, 0x69, 0xbb, 0xd6, 0x61, 0xa3,     0xfe, 0x19, 0x2b, 0x7d, 0x87, 0x92, 0xad, 0xec, 0x2f, 0x71, 0x93, 0xae, 0xe9, 0x20, 0x60, 0xa0,     0xfb, 0x16, 0x3a, 0x4e, 0xd2, 0x6d, 0xb7, 0xc2, 0x5d, 0xe7, 0x32, 0x56, 0xfa, 0x15, 0x3f, 0x41,     0xc3, 0x5e, 0xe2, 0x3d, 0x47, 0xc9, 0x40, 0xc0, 0x5b, 0xed, 0x2c, 0x74, 0x9c, 0xbf, 0xda, 0x75,     0x9f, 0xba, 0xd5, 0x64, 0xac, 0xef, 0x2a, 0x7e, 0x82, 0x9d, 0xbc, 0xdf, 0x7a, 0x8e, 0x89, 0x80,     0x9b, 0xb6, 0xc1, 0x58, 0xe8, 0x23, 0x65, 0xaf, 0xea, 0x25, 0x6f, 0xb1, 0xc8, 0x43, 0xc5, 0x54,     0xfc, 0x1f, 0x21, 0x63, 0xa5, 0xf4, 0x07, 0x09, 0x1b, 0x2d, 0x77, 0x99, 0xb0, 0xcb, 0x46, 0xca,     0x45, 0xcf, 0x4a, 0xde, 0x79, 0x8b, 0x86, 0x91, 0xa8, 0xe3, 0x3e, 0x42, 0xc6, 0x51, 0xf3, 0x0e,     0x12, 0x36, 0x5a, 0xee, 0x29, 0x7b, 0x8d, 0x8c, 0x8f, 0x8a, 0x85, 0x94, 0xa7, 0xf2, 0x0d, 0x17,     0x39, 0x4b, 0xdd, 0x7c, 0x84, 0x97, 0xa2, 0xfd, 0x1c, 0x24, 0x6c, 0xb4, 0xc7, 0x52, 0xf6, 0x01,    0x03, 0x05, 0x0f, 0x11, 0x33, 0x55, 0xff, 0x1a, 0x2e, 0x72, 0x96, 0xa1, 0xf8, 0x13, 0x35,     0x5f, 0xe1, 0x38, 0x48, 0xd8, 0x73, 0x95, 0xa4, 0xf7, 0x02, 0x06, 0x0a, 0x1e, 0x22, 0x66, 0xaa,     0xe5, 0x34, 0x5c, 0xe4, 0x37, 0x59, 0xeb, 0x26, 0x6a, 0xbe, 0xd9, 0x70, 0x90, 0xab, 0xe6, 0x31,     0x53, 0xf5, 0x04, 0x0c, 0x14, 0x3c, 0x44, 0xcc, 0x4f, 0xd1, 0x68, 0xb8, 0xd3, 0x6e, 0xb2, 0xcd,     0x4c, 0xd4, 0x67, 0xa9, 0xe0, 0x3b, 0x4d, 0xd7, 0x62, 0xa6, 0xf1, 0x08, 0x18, 0x28, 0x78, 0x88,     0x83, 0x9e, 0xb9, 0xd0, 0x6b, 0xbd, 0xdc, 0x7f, 0x81, 0x98, 0xb3, 0xce, 0x49, 0xdb, 0x76, 0x9a,     0xb5, 0xc4, 0x57, 0xf9, 0x10, 0x30, 0x50, 0xf0, 0x0b, 0x1d, 0x27, 0x69, 0xbb, 0xd6, 0x61, 0xa3,     0xfe, 0x19, 0x2b, 0x7d, 0x87, 0x92, 0xad, 0xec, 0x2f, 0x71, 0x93, 0xae, 0xe9, 0x20, 0x60, 0xa0,     0xfb, 0x16, 0x3a, 0x4e, 0xd2, 0x6d, 0xb7, 0xc2, 0x5d, 0xe7, 0x32, 0x56, 0xfa, 0x15, 0x3f, 0x41,     0xc3, 0x5e, 0xe2, 0x3d, 0x47, 0xc9, 0x40, 0xc0, 0x5b, 0xed, 0x2c, 0x74, 0x9c, 0xbf, 0xda, 0x75,     0x9f, 0xba, 0xd5, 0x64, 0xac, 0xef, 0x2a, 0x7e, 0x82, 0x9d, 0xbc, 0xdf, 0x7a, 0x8e, 0x89, 0x80,     0x9b, 0xb6, 0xc1, 0x58, 0xe8, 0x23, 0x65, 0xaf, 0xea, 0x25, 0x6f, 0xb1, 0xc8, 0x43, 0xc5, 0x54,     0xfc, 0x1f, 0x21, 0x63, 0xa5, 0xf4, 0x07, 0x09, 0x1b, 0x2d, 0x77, 0x99, 0xb0, 0xcb, 0x46, 0xca,     0x45, 0xcf, 0x4a, 0xde, 0x79, 0x8b, 0x86, 0x91, 0xa8, 0xe3, 0x3e, 0x42, 0xc6, 0x51, 0xf3, 0x0e,     0x12, 0x36, 0x5a, 0xee, 0x29, 0x7b, 0x8d, 0x8c, 0x8f, 0x8a, 0x85, 0x94, 0xa7, 0xf2, 0x0d, 0x17,     0x39, 0x4b, 0xdd, 0x7c, 0x84, 0x97, 0xa2, 0xfd, 0x1c, 0x24, 0x6c, 0xb4, 0xc7, 0x52, 0xf6, 0x01};
		if(x==0||y==0)
		{
			return 0;
		}else{
			set<int>::iterator it;
			it=table.begin();
			for(int i=0;i<x;i++)
			{
				it++;
			}
			x=*it;
			it=table.begin();
			for(int i=0;i<y;i++)
			{
				it++;
			}
			y=*it;

			it=table.begin();
			for(int i=0;i<x+y+256;i++)
			{
				it++;
			}
			x=*it;
			return x;


		}
	}

		static bool isArrayEmpty(vector<int>& dist) {
			for(int i = 0; i < dist.size(); i++) {
				if (dist[i] != 0)
					return false;
			}
			return true;
		}

	static bool isNumber(string i){
		stringstream stream;
		int ii;
		stream<<i;
		if(!(stream >> ii))
		{
			return false;
		}
		return true;

	}




};



#endif
