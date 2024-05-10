#include <Utils/Utils.h>

#include <memory>

DODO_BEGIN_NAMESPACE

class Application {
    public:
        static std::unique_ptr<Application> Create();

        void Init();
};

DODO_END_NAMESPACE