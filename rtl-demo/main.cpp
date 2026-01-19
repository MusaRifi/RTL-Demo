
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


    rtl::RObject callDefaultCtor(rtl::Record& pClass)
    {
        // Materialize the default constructor.
        rtl::constructor<> personCtor = pClass.ctorT<>();

        // Default ctor is gauranteed to be found, no need to check before calling.
        auto [err, person] = personCtor(rtl::alloc::Stack);
        if (err != rtl::error::None) {
            die(err);
        }
        else {
            std::cout << "\n[rtl] construction successful, returning the object.\n";
        }
        return std::move(person);
    }


    rtl::RObject callOverloadedCtor(rtl::Record& pClass)
    {
        // Materialize the overloaded constructor.
        rtl::constructor<std::string, int> personCtor = pClass.ctorT<std::string, int>();
        if (!personCtor) {  // Constructor with expected signature not found.
            die(personCtor.get_init_error());
        }

        auto [err, person] = personCtor(rtl::alloc::Heap, "Bernard", 42);
        if (err != rtl::error::None) {
            die(err);
        }
        else {
            std::cout << "\n[rtl] construction successful, returning the object.";
        }
        return std::move(person);
    }


    std::string callGetName_knownTypes(rtl::Method& pMethod) 
    {
        // Known 'Person' & return-type.
        rtl::method<Person, std::string()> getName = pMethod.targetT<Person>().argsT()
                                                            .returnT<std::string>();
        if (!getName) {
            die(getName.get_init_error());
        }

        // rtl::method<> works for both- rvalue/lvalue.
        return getName(Person("Alex", 10))();    // rvalue, destroyed right after the call.
    }


    void callSetName_erasedTargetKnownReturn(rtl::Method& pMethod, rtl::RObject& pTarget)
    {
        // erased 'Person' & Known return-type.
        rtl::method<rtl::RObject, void(std::string)> setName = pMethod.targetT()
                                                                      .argsT<std::string>()
                                                                      .returnT<void>();
        if (!setName) {
            die(setName.get_init_error());
        }

        auto [err, robj] = setName(pTarget)("Walter");   // robj: type std::optional<>, empty since the function is void.
        if (err != rtl::error::None) {
            die(err);
        }
    }


    std::string callGetName_erasedTargetKnownReturn(rtl::Method& pMethod, rtl::RObject& pTarget)
    {
        // erased 'Person' & Known return-type.
        rtl::method<rtl::RObject, std::string()> getName = pMethod.targetT().argsT()
                                                                  .returnT<std::string>();
        if (!getName) {
            die(getName.get_init_error());
        }

        auto [err, robj] = getName(pTarget)(); // robj: type std::optional<std::string>, nont empty.
        if (err != rtl::error::None) {
            die(err);
        }
        return robj.value();
    }


    rtl::RObject callGetName_erasedTargetAndReturn(rtl::Method& pMethod, rtl::RObject& pTarget)
    {
        // Known 'Person' & return-type.
        rtl::method<rtl::RObject, rtl::Return()> getName = pMethod.targetT().argsT().returnT();

        if (!getName) {
            die(getName.get_init_error());
        }

        auto [err, robj] = getName(pTarget)(); // robj: type std::optional<std::string>, nont empty.
        if (err != rtl::error::None) {
            die(err);
        }
        return std::move(robj);
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

        std::string nameStr = callGetName_knownTypes(*oGetName);
        std::cout << "\n[rtl] getName() returned: \"" << nameStr << "\"";

        std::cout << "\n[rtl] Creating instance on stack.\n";
        {
            rtl::RObject person = callDefaultCtor(*classPerson);

            std::cout << "\n[rtl] Looking up reflected method: setName";
            std::optional<rtl::Method> oSetName = classPerson->getMethod("setName");
            if (!oSetName) {
                die("Method \"setName\" not found.");
            }
            std::cout << " [ok]";

            std::cout << "\n[rtl] Calling setName() with erased target & known return type.\n";
            callSetName_erasedTargetKnownReturn(*oSetName, person);

            std::cout << "\n[rtl] Calling getName() on mutated object.\n";

            std::string name = callGetName_erasedTargetKnownReturn(*oGetName, person);
            std::cout << "\n[rtl] getName() returned: \"" << name << "\"\n";
        }
        std::cout << "\n[rtl] Stack instance destroyed.";

        std::cout << "\n[rtl] Creating instance on Heap.\n";
        {
            rtl::RObject person = callOverloadedCtor(*classPerson);

            std::cout << "\n[rtl] Calling getName() with erased target & return type.\n";
            rtl::RObject retStr = callGetName_erasedTargetAndReturn(*oGetName, person);

            if (retStr.canViewAs<std::string>()) {
                const std::string& name = retStr.view<std::string>()->get();
                std::cout << "\n[rtl] getName() returned: \"" << name << "\"";
            }

            std::cout << "\n[rtl] Cloning reflected object on stack.\n";
            {
                auto [err, personCp] = person.clone<rtl::alloc::Stack>();
                if (err != rtl::error::None) {
                    die(err);
                }
                else {
                    std::cout << "\n[rtl] cloning successful.";
                }

                std::cout << "\n[rtl] Calling getName() on cloned object.\n";
                rtl::RObject retCpStr = callGetName_erasedTargetAndReturn(*oGetName, personCp);

                if (retCpStr.canViewAs<std::string>()) {
                    const std::string& name = retCpStr.view<std::string>()->get();
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