
#include <iostream>
#include <rtl_access.h>    // Reflection access interface.

#include "Person/Person.h"
#include "MyReflection/MyReflection.h"

namespace
{
    [[noreturn]] void die(rtl::error err)
    {
        std::cerr << "\n[error] " << rtl::to_string(err) << "\n";
        std::exit(0);
    }

    [[noreturn]] void die(const char* msg)
    {
        std::cerr << "\n[error] " << msg << "\n";
        std::exit(0);
    }
}

int main()
{
    std::cout << "\n=== RTL Reflection Demo ===\n";
    {
        std::cout << "\n[rtl] Looking up reflected class: Person";
        std::optional<rtl::Record> classPerson = cxx::mirror().getRecord("Person");
        if (!classPerson) {
            die("Class \"Person\" not found.");
        }
        std::cout << " [ok]";

        std::cout << "\n[rtl] Looking up reflected method: getName";
        std::optional<rtl::Method> oGetName = classPerson->getMethod("getName");
        if (!oGetName) {
            die("Method \"getName\" not found.");
        }
        std::cout << " [ok]";

        std::cout << "\n[rtl] Calling getName() with complete type awareness.\n";
        // Typed target & return.
        rtl::method<Person, std::string()> getName = oGetName->targetT<Person>()
                                                             .argsT().returnT<std::string>();
        if (!getName) {
            die(getName.get_init_error());
        }

        // rtl::method<> works for both- rvalue/lvalue.
        std::string nameStr = getName(Person("Alex", 10))();
        std::cout << "\n[rtl] getName() returned: \"" << nameStr << "\"";

        rtl::constructor<> defaultCtor = classPerson->ctorT<>();
        std::cout << "\n[rtl] Creating instance on stack.\n";
        {
            auto [err0, person] = defaultCtor(rtl::alloc::Stack);
            if (err0 != rtl::error::None) {
                die(err0);
            }
            else {
                std::cout << "\n[rtl] construction successful.";
            }

            std::cout << "\n[rtl] Looking up reflected method: setName";
            std::optional<rtl::Method> oSetName = classPerson->getMethod("setName");
            if (!oSetName) {
                die("Method \"setName\" not found.");
            }
            std::cout << " [ok]";

            rtl::method<rtl::RObject, void(std::string)> setName = oSetName->targetT()
                                                                           .argsT<std::string>().returnT<void>();
            if (!setName) {
                die(setName.get_init_error());
            }

            std::cout << "\n[rtl] Calling setName() with target erased & known return type.\n";
            auto [err1, ret] = setName(person)("Walter");   // Returns rtl::error & std::nullopt.
            if (err1 != rtl::error::None) {
                die(err1);
            }

            rtl::method<rtl::RObject, std::string()> getName = oGetName->targetT().argsT().returnT<std::string>();
            if (!getName) {
                die(getName.get_init_error());
            }

            std::cout << "\n[rtl] Calling getName() on mutated object.\n";
            auto [err2, optnl] = getName(person)(); // Returns rtl::error & std::optional<std::string>.
            if (err2 != rtl::error::None) {
                die(err2);
            }
            std::cout << "\n[rtl] getName() returned: \"" << optnl.value() << "\"\n";
        }
        std::cout << "\n[rtl] Stack instance destroyed.";

        rtl::constructor<std::string, int> personCtor = classPerson->ctorT<std::string, int>();
        std::cout << "\n[rtl] Creating instance on Heap.\n";
        {
            auto [err0, person0] = personCtor(rtl::alloc::Heap, "Bernard", 42);
            if (err0 != rtl::error::None) {
                die(err0);
            }
            else {
                std::cout << "\n[rtl] construction successful.";
            }

            rtl::method<rtl::RObject, rtl::Return()> getName = oGetName->targetT().argsT().returnT();
            if (!getName) {
                die(getName.get_init_error());
            }

            std::cout << "\n[rtl] Calling getName() with erased target & return type.\n";
            auto [err, ret] = getName(person0)();   // Returns rtl::error & rtl::RObject.
            if (err != rtl::error::None) {
                die(err);
            }
            if (ret.canViewAs<std::string>()) {
                const std::string& name = ret.view<std::string>()->get();
                std::cout << "\n[rtl] getName() returned: \"" << name << "\"";
            }

            std::cout << "\n[rtl] Cloning reflected object on stack.\n";
            {
                auto [err1, person1] = person0.clone<rtl::alloc::Stack>();
                if (err1 != rtl::error::None) {
                    die(err1);
                }
                else {
                    std::cout << "\n[rtl] cloning successful.";
                }

                std::cout << "\n[rtl] Calling getName() on cloned object.\n";
                auto [err, ret] = getName(person1)();
                if (err != rtl::error::None) {
                    die(err);
                }

                if (ret.canViewAs<std::string>()) {
                    const std::string& name = ret.view<std::string>()->get();
                    std::cout << "\n[rtl] getName() returned: \"" << name << "\"\n";
                }
            }
            std::cout << "\n[rtl] Stack cloned instance destroyed.\n";
        }
        std::cout << "\n[rtl] Heap instance destroyed.";
    }
    std::cout << "\n\n====== Demo Complete ======\n";
    return 0;
}