#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <cstring>
#include <cassert>

using namespace std;

class Domain {
public:
    Domain(const string& name)
        :rname_{string{name.rbegin(), name.rend()}+ "."} {
    }
    bool operator==(const Domain& other) const {
        return rname_ == other.rname_;
    }
    bool IsSubdomain(const Domain& domain) const {
        const string& s = domain.GetReverse();
        size_t sz = s.size();
        if(rname_.size() < sz) {
            return false;
        }
        return memcmp(domain.GetReverse().data(), rname_.data(), sz) == 0;
    }
    const string& GetReverse() const {
        return rname_;
    }
private:
    string rname_; // перевёрнутая строка с добавленным в конец символом '.'
};

class DomainChecker {
public:
    template <typename Iterator>
    DomainChecker(Iterator first, Iterator last) {
        forbidden_domains_.reserve(distance(first, last));
        while(first != last) {
            forbidden_domains_.emplace_back(*first);
            ++first;
        }
        sort(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& one, const Domain& two) {
            return one.GetReverse() < two.GetReverse();
        });
        auto unique_last = unique(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& one, const Domain& two) {
            return two.IsSubdomain(one);
        });
        forbidden_domains_.erase(unique_last, forbidden_domains_.end());
    }
    bool IsForbidden(const Domain& sub_domain) const {
        auto upper = upper_bound(forbidden_domains_.begin(),
                                 forbidden_domains_.end(),
                                 sub_domain, [](const Domain& one, const Domain& two) {
            return one.GetReverse() < two.GetReverse();
        });
        if(upper == forbidden_domains_.begin()) {
            return false;
        }
        return sub_domain.IsSubdomain(*(upper-1));
    }
private:
    vector<Domain> forbidden_domains_;
};

std::vector<Domain> ReadDomains(istream& input, size_t amount) {
    std::vector<Domain> v;
    v.reserve(amount);
    string line;
    while(amount--) {
        getline(input, line);
        v.emplace_back(move(Domain{line}));
    }
    return v;
}

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

bool test() {
    {
        Domain d1 = Domain("124");
        string s = "456";
        Domain d2 = Domain(s);
    }
    {
        string s1 = "12.34";
        string s2 = "12.34";

        if(!memcmp(s1.data(), s2.data(), s1.size())) {
            cout << "s1 == s2" << endl;
        }
        else {
            cout << "s1 != s2" << endl;
        }
    }
    {
        Domain d1("aaa");
        Domain d1_sub("aaa.aaa.aaa");
        assert(d1_sub.IsSubdomain(d1));
        assert(d1.IsSubdomain(d1_sub) == false);
    }
    {
        vector<Domain> forbidden_domains_ = {
            {"aaa.aaa"},
            {"bbb"},
            {"bbb.bbb.bbb"},
            {"aaa.aaa.aaa"},
            {"bbb.bbb.bbb.bbb"},
            {"aaa"}
        };
        sort(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& one, const Domain& two) {
            return lexicographical_compare(one.GetReverse().begin(), one.GetReverse().end(),
                                           two.GetReverse().begin(), one.GetReverse().end());
        });
        auto last = unique(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& one, const Domain& two) {
            return two.IsSubdomain(one);
        });
        forbidden_domains_.erase(last, forbidden_domains_.end());
        vector<Domain> vcmp = {
            {"aaa"},
            {"bbb"}
        };
        assert(vcmp == forbidden_domains_);
    }
    {
        vector<Domain> forbidden_domains_ = {
            {"aa"},
            {"aa.a"},
            {"bbb"},
            {"cc.cc"}
        };
        sort(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& one, const Domain& two) {
            return lexicographical_compare(one.GetReverse().begin(), one.GetReverse().end(),
                                           two.GetReverse().begin(), one.GetReverse().end());
        });
        auto last = unique(forbidden_domains_.begin(), forbidden_domains_.end(), [](const Domain& one, const Domain& two) {
            return two.IsSubdomain(one);
        });
        forbidden_domains_.erase(last, forbidden_domains_.end());

        auto lambda = [&forbidden_domains_](const Domain& sd) {
            auto upper = upper_bound(forbidden_domains_.begin(),
                                     forbidden_domains_.end(),
                                     sd, [](const Domain& one, const Domain& two) {
                return one.GetReverse() < two.GetReverse();
            });
            if(upper == forbidden_domains_.begin()) {
                cout << "upper beg: " << upper->GetReverse() <<  endl;
            }
            else if(upper == forbidden_domains_.end()) {
                cout << "upper end" << endl;
            }
            else {
                cout << "upper find " << upper->GetReverse() << '\n';
            }

            return true;
        };
        Domain sd1("a.a");
        lambda(sd1);
        Domain sd2("aa");
        lambda(sd2);
        cout << "--" << endl;
    }

    return true;
}

int main() {
    if(false) {
        if(test()) {
            cout << "test Ok" << endl;
        }
    }

    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
