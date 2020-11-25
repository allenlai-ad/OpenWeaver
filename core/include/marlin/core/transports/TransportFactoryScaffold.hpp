#ifndef MARLIN_CORE_TRANSPORTFACTORYSCAFFOLD
#define MARLIN_CORE_TRANSPORTFACTORYSCAFFOLD

#include <marlin/core/TransportManager.hpp>
#include <marlin/core/SocketAddress.hpp>

namespace marlin {
namespace core {

template<
	typename TransportFactoryType,
	typename TransportType,
	typename ListenDelegate,
	typename TransportDelegate,
	typename BaseTransportFactoryType,
	typename BaseTransportType
>
class TransportFactoryScaffold {
protected:
	BaseTransportFactoryType base_factory;

	ListenDelegate* delegate = nullptr;
	TransportManager<TransportType> transport_manager;

public:
	template<typename ...Args>
	TransportFactoryScaffold(Args&&... args);

	// BaseTransportFactoryType delegate
	bool should_accept(SocketAddress const& addr);
	void did_create_transport(BaseTransportType base_transport);

	core::SocketAddress addr;

	int bind(SocketAddress const& addr);
	int listen(ListenDelegate& delegate);
	template<typename... Args>
	int dial(SocketAddress const& addr, ListenDelegate& delegate, Args&&... args);

	TransportType* get_transport(SocketAddress const& addr);
};


template<
	typename ListenDelegate,
	typename TransportDelegate,
	template<typename, typename> typename BaseTransportFactoryTemplate,
	template<typename> typename BaseTransportTemplate,
	template<typename, typename, template<typename, typename> typename, template<typename> typename, typename...> typename TransportFactoryTemplate,
	template<typename, template<typename> typename, typename...> typename TransportTemplate,
	typename... TArgs
>
using SugaredTransportFactoryScaffold = TransportFactoryScaffold<
	TransportFactoryTemplate<ListenDelegate, TransportDelegate, BaseTransportFactoryTemplate, BaseTransportTemplate, TArgs...>,
	TransportTemplate<TransportDelegate, BaseTransportTemplate, TArgs...>,
	ListenDelegate,
	TransportDelegate,
	BaseTransportFactoryTemplate<
		TransportFactoryTemplate<ListenDelegate, TransportDelegate, BaseTransportFactoryTemplate, BaseTransportTemplate, TArgs...>,
		TransportTemplate<TransportDelegate, BaseTransportTemplate, TArgs...>
	>,
	BaseTransportTemplate<TransportTemplate<TransportDelegate, BaseTransportTemplate, TArgs...>>&
>;


}  // namespace core
}  // namespace marlin

#include "TransportFactoryScaffold.ipp"

#endif  // MARLIN_CORE_TRANSPORTFACTORYSCAFFOLD
