#pragma once

#include <string>
#include <iostream>

class Person
{
    int m_age;
    std::string m_name;

public:

    Person()
        : m_name("empty")
        , m_age(0) {
        std::cout << "\n[ctor] Person() called, default name: \"" << m_name << "\"";
    }

    Person(Person&& pOther) noexcept
        : m_name(pOther.m_name)
        , m_age(pOther.m_age) {
        std::cout << "\n[mvctor] Person(Person&&) called, move name: \"" << m_name << "\"";
    }

    Person(const Person& pOther) noexcept
        : m_name(pOther.m_name)
        , m_age(pOther.m_age) {
        std::cout << "\n[cpctor] Person(const Person&) called, copy name: \"" << m_name << "\"";
    }

    Person(std::string pName, int pAge) noexcept
        : m_age(pAge)
        , m_name(pName) {
        std::cout << "\n[ctor] Person(std::string, int) called, init name: \"" << m_name << "\"";
    }

    ~Person() noexcept {
        std::cout << "\n[dtor] ~Person() called, destroying: \"" << m_name << "\"";
    }

    std::string getName() noexcept {
        std::cout << "\n[method] Person::getName() called.";
        return m_name;
    }

    void setName(std::string pName) noexcept {
        std::cout << "\n[method] Person::setName(), \""<<m_name<< "\" changed to: \""<< pName << "\"";
        m_name = pName;
    }
};