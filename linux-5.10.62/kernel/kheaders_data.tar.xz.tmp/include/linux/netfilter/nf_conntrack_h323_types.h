/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _NF_CONNTRACK_H323_TYPES_H
#define _NF_CONNTRACK_H323_TYPES_H

typedef struct TransportAddress_ipAddress {	
	int options;		
	unsigned int ip;
} TransportAddress_ipAddress;

typedef struct TransportAddress_ip6Address {	
	int options;		
	unsigned int ip;
} TransportAddress_ip6Address;

typedef struct TransportAddress {	
	enum {
		eTransportAddress_ipAddress,
		eTransportAddress_ipSourceRoute,
		eTransportAddress_ipxAddress,
		eTransportAddress_ip6Address,
		eTransportAddress_netBios,
		eTransportAddress_nsap,
		eTransportAddress_nonStandardAddress,
	} choice;
	union {
		TransportAddress_ipAddress ipAddress;
		TransportAddress_ip6Address ip6Address;
	};
} TransportAddress;

typedef struct DataProtocolCapability {	
	enum {
		eDataProtocolCapability_nonStandard,
		eDataProtocolCapability_v14buffered,
		eDataProtocolCapability_v42lapm,
		eDataProtocolCapability_hdlcFrameTunnelling,
		eDataProtocolCapability_h310SeparateVCStack,
		eDataProtocolCapability_h310SingleVCStack,
		eDataProtocolCapability_transparent,
		eDataProtocolCapability_segmentationAndReassembly,
		eDataProtocolCapability_hdlcFrameTunnelingwSAR,
		eDataProtocolCapability_v120,
		eDataProtocolCapability_separateLANStack,
		eDataProtocolCapability_v76wCompression,
		eDataProtocolCapability_tcp,
		eDataProtocolCapability_udp,
	} choice;
} DataProtocolCapability;

typedef struct DataApplicationCapability_application {	
	enum {
		eDataApplicationCapability_application_nonStandard,
		eDataApplicationCapability_application_t120,
		eDataApplicationCapability_application_dsm_cc,
		eDataApplicationCapability_application_userData,
		eDataApplicationCapability_application_t84,
		eDataApplicationCapability_application_t434,
		eDataApplicationCapability_application_h224,
		eDataApplicationCapability_application_nlpid,
		eDataApplicationCapability_application_dsvdControl,
		eDataApplicationCapability_application_h222DataPartitioning,
		eDataApplicationCapability_application_t30fax,
		eDataApplicationCapability_application_t140,
		eDataApplicationCapability_application_t38fax,
		eDataApplicationCapability_application_genericDataCapability,
	} choice;
	union {
		DataProtocolCapability t120;
	};
} DataApplicationCapability_application;

typedef struct DataApplicationCapability {	
	int options;		
	DataApplicationCapability_application application;
} DataApplicationCapability;

typedef struct DataType {	
	enum {
		eDataType_nonStandard,
		eDataType_nullData,
		eDataType_videoData,
		eDataType_audioData,
		eDataType_data,
		eDataType_encryptionData,
		eDataType_h235Control,
		eDataType_h235Media,
		eDataType_multiplexedStream,
	} choice;
	union {
		DataApplicationCapability data;
	};
} DataType;

typedef struct UnicastAddress_iPAddress {	
	int options;		
	unsigned int network;
} UnicastAddress_iPAddress;

typedef struct UnicastAddress_iP6Address {	
	int options;		
	unsigned int network;
} UnicastAddress_iP6Address;

typedef struct UnicastAddress {	
	enum {
		eUnicastAddress_iPAddress,
		eUnicastAddress_iPXAddress,
		eUnicastAddress_iP6Address,
		eUnicastAddress_netBios,
		eUnicastAddress_iPSourceRouteAddress,
		eUnicastAddress_nsap,
		eUnicastAddress_nonStandardAddress,
	} choice;
	union {
		UnicastAddress_iPAddress iPAddress;
		UnicastAddress_iP6Address iP6Address;
	};
} UnicastAddress;

typedef struct H245_TransportAddress {	
	enum {
		eH245_TransportAddress_unicastAddress,
		eH245_TransportAddress_multicastAddress,
	} choice;
	union {
		UnicastAddress unicastAddress;
	};
} H245_TransportAddress;

typedef struct H2250LogicalChannelParameters {	
	enum {
		eH2250LogicalChannelParameters_nonStandard = (1 << 31),
		eH2250LogicalChannelParameters_associatedSessionID =
		    (1 << 30),
		eH2250LogicalChannelParameters_mediaChannel = (1 << 29),
		eH2250LogicalChannelParameters_mediaGuaranteedDelivery =
		    (1 << 28),
		eH2250LogicalChannelParameters_mediaControlChannel =
		    (1 << 27),
		eH2250LogicalChannelParameters_mediaControlGuaranteedDelivery
		    = (1 << 26),
		eH2250LogicalChannelParameters_silenceSuppression = (1 << 25),
		eH2250LogicalChannelParameters_destination = (1 << 24),
		eH2250LogicalChannelParameters_dynamicRTPPayloadType =
		    (1 << 23),
		eH2250LogicalChannelParameters_mediaPacketization = (1 << 22),
		eH2250LogicalChannelParameters_transportCapability =
		    (1 << 21),
		eH2250LogicalChannelParameters_redundancyEncoding = (1 << 20),
		eH2250LogicalChannelParameters_source = (1 << 19),
	} options;
	H245_TransportAddress mediaChannel;
	H245_TransportAddress mediaControlChannel;
} H2250LogicalChannelParameters;

typedef struct OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters {	
	enum {
		eOpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters_h222LogicalChannelParameters,
		eOpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters_h223LogicalChannelParameters,
		eOpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters_v76LogicalChannelParameters,
		eOpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters_h2250LogicalChannelParameters,
		eOpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters_none,
	} choice;
	union {
		H2250LogicalChannelParameters h2250LogicalChannelParameters;
	};
} OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters;

typedef struct OpenLogicalChannel_forwardLogicalChannelParameters {	
	enum {
		eOpenLogicalChannel_forwardLogicalChannelParameters_portNumber
		    = (1 << 31),
		eOpenLogicalChannel_forwardLogicalChannelParameters_forwardLogicalChannelDependency
		    = (1 << 30),
		eOpenLogicalChannel_forwardLogicalChannelParameters_replacementFor
		    = (1 << 29),
	} options;
	DataType dataType;
	OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters
	    multiplexParameters;
} OpenLogicalChannel_forwardLogicalChannelParameters;

typedef struct OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters {	
	enum {
		eOpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters_h223LogicalChannelParameters,
		eOpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters_v76LogicalChannelParameters,
		eOpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters_h2250LogicalChannelParameters,
	} choice;
	union {
		H2250LogicalChannelParameters h2250LogicalChannelParameters;
	};
} OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters;

typedef struct OpenLogicalChannel_reverseLogicalChannelParameters {	
	enum {
		eOpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters
		    = (1 << 31),
		eOpenLogicalChannel_reverseLogicalChannelParameters_reverseLogicalChannelDependency
		    = (1 << 30),
		eOpenLogicalChannel_reverseLogicalChannelParameters_replacementFor
		    = (1 << 29),
	} options;
	OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters
	    multiplexParameters;
} OpenLogicalChannel_reverseLogicalChannelParameters;

typedef struct NetworkAccessParameters_networkAddress {	
	enum {
		eNetworkAccessParameters_networkAddress_q2931Address,
		eNetworkAccessParameters_networkAddress_e164Address,
		eNetworkAccessParameters_networkAddress_localAreaAddress,
	} choice;
	union {
		H245_TransportAddress localAreaAddress;
	};
} NetworkAccessParameters_networkAddress;

typedef struct NetworkAccessParameters {	
	enum {
		eNetworkAccessParameters_distribution = (1 << 31),
		eNetworkAccessParameters_externalReference = (1 << 30),
		eNetworkAccessParameters_t120SetupProcedure = (1 << 29),
	} options;
	NetworkAccessParameters_networkAddress networkAddress;
} NetworkAccessParameters;

typedef struct OpenLogicalChannel {	
	enum {
		eOpenLogicalChannel_reverseLogicalChannelParameters =
		    (1 << 31),
		eOpenLogicalChannel_separateStack = (1 << 30),
		eOpenLogicalChannel_encryptionSync = (1 << 29),
	} options;
	OpenLogicalChannel_forwardLogicalChannelParameters
	    forwardLogicalChannelParameters;
	OpenLogicalChannel_reverseLogicalChannelParameters
	    reverseLogicalChannelParameters;
	NetworkAccessParameters separateStack;
} OpenLogicalChannel;

typedef struct Setup_UUIE_fastStart {	
	int count;
	OpenLogicalChannel item[30];
} Setup_UUIE_fastStart;

typedef struct Setup_UUIE {	
	enum {
		eSetup_UUIE_h245Address = (1 << 31),
		eSetup_UUIE_sourceAddress = (1 << 30),
		eSetup_UUIE_destinationAddress = (1 << 29),
		eSetup_UUIE_destCallSignalAddress = (1 << 28),
		eSetup_UUIE_destExtraCallInfo = (1 << 27),
		eSetup_UUIE_destExtraCRV = (1 << 26),
		eSetup_UUIE_callServices = (1 << 25),
		eSetup_UUIE_sourceCallSignalAddress = (1 << 24),
		eSetup_UUIE_remoteExtensionAddress = (1 << 23),
		eSetup_UUIE_callIdentifier = (1 << 22),
		eSetup_UUIE_h245SecurityCapability = (1 << 21),
		eSetup_UUIE_tokens = (1 << 20),
		eSetup_UUIE_cryptoTokens = (1 << 19),
		eSetup_UUIE_fastStart = (1 << 18),
		eSetup_UUIE_mediaWaitForConnect = (1 << 17),
		eSetup_UUIE_canOverlapSend = (1 << 16),
		eSetup_UUIE_endpointIdentifier = (1 << 15),
		eSetup_UUIE_multipleCalls = (1 << 14),
		eSetup_UUIE_maintainConnection = (1 << 13),
		eSetup_UUIE_connectionParameters = (1 << 12),
		eSetup_UUIE_language = (1 << 11),
		eSetup_UUIE_presentationIndicator = (1 << 10),
		eSetup_UUIE_screeningIndicator = (1 << 9),
		eSetup_UUIE_serviceControl = (1 << 8),
		eSetup_UUIE_symmetricOperationRequired = (1 << 7),
		eSetup_UUIE_capacity = (1 << 6),
		eSetup_UUIE_circuitInfo = (1 << 5),
		eSetup_UUIE_desiredProtocols = (1 << 4),
		eSetup_UUIE_neededFeatures = (1 << 3),
		eSetup_UUIE_desiredFeatures = (1 << 2),
		eSetup_UUIE_supportedFeatures = (1 << 1),
		eSetup_UUIE_parallelH245Control = (1 << 0),
	} options;
	TransportAddress h245Address;
	TransportAddress destCallSignalAddress;
	TransportAddress sourceCallSignalAddress;
	Setup_UUIE_fastStart fastStart;
} Setup_UUIE;

typedef struct CallProceeding_UUIE_fastStart {	
	int count;
	OpenLogicalChannel item[30];
} CallProceeding_UUIE_fastStart;

typedef struct CallProceeding_UUIE {	
	enum {
		eCallProceeding_UUIE_h245Address = (1 << 31),
		eCallProceeding_UUIE_callIdentifier = (1 << 30),
		eCallProceeding_UUIE_h245SecurityMode = (1 << 29),
		eCallProceeding_UUIE_tokens = (1 << 28),
		eCallProceeding_UUIE_cryptoTokens = (1 << 27),
		eCallProceeding_UUIE_fastStart = (1 << 26),
		eCallProceeding_UUIE_multipleCalls = (1 << 25),
		eCallProceeding_UUIE_maintainConnection = (1 << 24),
		eCallProceeding_UUIE_fastConnectRefused = (1 << 23),
		eCallProceeding_UUIE_featureSet = (1 << 22),
	} options;
	TransportAddress h245Address;
	CallProceeding_UUIE_fastStart fastStart;
} CallProceeding_UUIE;

typedef struct Connect_UUIE_fastStart {	
	int count;
	OpenLogicalChannel item[30];
} Connect_UUIE_fastStart;

typedef struct Connect_UUIE {	
	enum {
		eConnect_UUIE_h245Address = (1 << 31),
		eConnect_UUIE_callIdentifier = (1 << 30),
		eConnect_UUIE_h245SecurityMode = (1 << 29),
		eConnect_UUIE_tokens = (1 << 28),
		eConnect_UUIE_cryptoTokens = (1 << 27),
		eConnect_UUIE_fastStart = (1 << 26),
		eConnect_UUIE_multipleCalls = (1 << 25),
		eConnect_UUIE_maintainConnection = (1 << 24),
		eConnect_UUIE_language = (1 << 23),
		eConnect_UUIE_connectedAddress = (1 << 22),
		eConnect_UUIE_presentationIndicator = (1 << 21),
		eConnect_UUIE_screeningIndicator = (1 << 20),
		eConnect_UUIE_fastConnectRefused = (1 << 19),
		eConnect_UUIE_serviceControl = (1 << 18),
		eConnect_UUIE_capacity = (1 << 17),
		eConnect_UUIE_featureSet = (1 << 16),
	} options;
	TransportAddress h245Address;
	Connect_UUIE_fastStart fastStart;
} Connect_UUIE;

typedef struct Alerting_UUIE_fastStart {	
	int count;
	OpenLogicalChannel item[30];
} Alerting_UUIE_fastStart;

typedef struct Alerting_UUIE {	
	enum {
		eAlerting_UUIE_h245Address = (1 << 31),
		eAlerting_UUIE_callIdentifier = (1 << 30),
		eAlerting_UUIE_h245SecurityMode = (1 << 29),
		eAlerting_UUIE_tokens = (1 << 28),
		eAlerting_UUIE_cryptoTokens = (1 << 27),
		eAlerting_UUIE_fastStart = (1 << 26),
		eAlerting_UUIE_multipleCalls = (1 << 25),
		eAlerting_UUIE_maintainConnection = (1 << 24),
		eAlerting_UUIE_alertingAddress = (1 << 23),
		eAlerting_UUIE_presentationIndicator = (1 << 22),
		eAlerting_UUIE_screeningIndicator = (1 << 21),
		eAlerting_UUIE_fastConnectRefused = (1 << 20),
		eAlerting_UUIE_serviceControl = (1 << 19),
		eAlerting_UUIE_capacity = (1 << 18),
		eAlerting_UUIE_featureSet = (1 << 17),
	} options;
	TransportAddress h245Address;
	Alerting_UUIE_fastStart fastStart;
} Alerting_UUIE;

typedef struct FacilityReason {	
	enum {
		eFacilityReason_routeCallToGatekeeper,
		eFacilityReason_callForwarded,
		eFacilityReason_routeCallToMC,
		eFacilityReason_undefinedReason,
		eFacilityReason_conferenceListChoice,
		eFacilityReason_startH245,
		eFacilityReason_noH245,
		eFacilityReason_newTokens,
		eFacilityReason_featureSetUpdate,
		eFacilityReason_forwardedElements,
		eFacilityReason_transportedInformation,
	} choice;
} FacilityReason;

typedef struct Facility_UUIE_fastStart {	
	int count;
	OpenLogicalChannel item[30];
} Facility_UUIE_fastStart;

typedef struct Facility_UUIE {	
	enum {
		eFacility_UUIE_alternativeAddress = (1 << 31),
		eFacility_UUIE_alternativeAliasAddress = (1 << 30),
		eFacility_UUIE_conferenceID = (1 << 29),
		eFacility_UUIE_callIdentifier = (1 << 28),
		eFacility_UUIE_destExtraCallInfo = (1 << 27),
		eFacility_UUIE_remoteExtensionAddress = (1 << 26),
		eFacility_UUIE_tokens = (1 << 25),
		eFacility_UUIE_cryptoTokens = (1 << 24),
		eFacility_UUIE_conferences = (1 << 23),
		eFacility_UUIE_h245Address = (1 << 22),
		eFacility_UUIE_fastStart = (1 << 21),
		eFacility_UUIE_multipleCalls = (1 << 20),
		eFacility_UUIE_maintainConnection = (1 << 19),
		eFacility_UUIE_fastConnectRefused = (1 << 18),
		eFacility_UUIE_serviceControl = (1 << 17),
		eFacility_UUIE_circuitInfo = (1 << 16),
		eFacility_UUIE_featureSet = (1 << 15),
		eFacility_UUIE_destinationInfo = (1 << 14),
		eFacility_UUIE_h245SecurityMode = (1 << 13),
	} options;
	TransportAddress alternativeAddress;
	FacilityReason reason;
	TransportAddress h245Address;
	Facility_UUIE_fastStart fastStart;
} Facility_UUIE;

typedef struct Progress_UUIE_fastStart {	
	int count;
	OpenLogicalChannel item[30];
} Progress_UUIE_fastStart;

typedef struct Progress_UUIE {	
	enum {
		eProgress_UUIE_h245Address = (1 << 31),
		eProgress_UUIE_h245SecurityMode = (1 << 30),
		eProgress_UUIE_tokens = (1 << 29),
		eProgress_UUIE_cryptoTokens = (1 << 28),
		eProgress_UUIE_fastStart = (1 << 27),
		eProgress_UUIE_multipleCalls = (1 << 26),
		eProgress_UUIE_maintainConnection = (1 << 25),
		eProgress_UUIE_fastConnectRefused = (1 << 24),
	} options;
	TransportAddress h245Address;
	Progress_UUIE_fastStart fastStart;
} Progress_UUIE;

typedef struct H323_UU_PDU_h323_message_body {	
	enum {
		eH323_UU_PDU_h323_message_body_setup,
		eH323_UU_PDU_h323_message_body_callProceeding,
		eH323_UU_PDU_h323_message_body_connect,
		eH323_UU_PDU_h323_message_body_alerting,
		eH323_UU_PDU_h323_message_body_information,
		eH323_UU_PDU_h323_message_body_releaseComplete,
		eH323_UU_PDU_h323_message_body_facility,
		eH323_UU_PDU_h323_message_body_progress,
		eH323_UU_PDU_h323_message_body_empty,
		eH323_UU_PDU_h323_message_body_status,
		eH323_UU_PDU_h323_message_body_statusInquiry,
		eH323_UU_PDU_h323_message_body_setupAcknowledge,
		eH323_UU_PDU_h323_message_body_notify,
	} choice;
	union {
		Setup_UUIE setup;
		CallProceeding_UUIE callProceeding;
		Connect_UUIE connect;
		Alerting_UUIE alerting;
		Facility_UUIE facility;
		Progress_UUIE progress;
	};
} H323_UU_PDU_h323_message_body;

typedef struct RequestMessage {	
	enum {
		eRequestMessage_nonStandard,
		eRequestMessage_masterSlaveDetermination,
		eRequestMessage_terminalCapabilitySet,
		eRequestMessage_openLogicalChannel,
		eRequestMessage_closeLogicalChannel,
		eRequestMessage_requestChannelClose,
		eRequestMessage_multiplexEntrySend,
		eRequestMessage_requestMultiplexEntry,
		eRequestMessage_requestMode,
		eRequestMessage_roundTripDelayRequest,
		eRequestMessage_maintenanceLoopRequest,
		eRequestMessage_communicationModeRequest,
		eRequestMessage_conferenceRequest,
		eRequestMessage_multilinkRequest,
		eRequestMessage_logicalChannelRateRequest,
	} choice;
	union {
		OpenLogicalChannel openLogicalChannel;
	};
} RequestMessage;

typedef struct OpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters {	
	enum {
		eOpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters_h222LogicalChannelParameters,
		eOpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters_h2250LogicalChannelParameters,
	} choice;
	union {
		H2250LogicalChannelParameters h2250LogicalChannelParameters;
	};
} OpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters;

typedef struct OpenLogicalChannelAck_reverseLogicalChannelParameters {	
	enum {
		eOpenLogicalChannelAck_reverseLogicalChannelParameters_portNumber
		    = (1 << 31),
		eOpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters
		    = (1 << 30),
		eOpenLogicalChannelAck_reverseLogicalChannelParameters_replacementFor
		    = (1 << 29),
	} options;
	OpenLogicalChannelAck_reverseLogicalChannelParameters_multiplexParameters
	    multiplexParameters;
} OpenLogicalChannelAck_reverseLogicalChannelParameters;

typedef struct H2250LogicalChannelAckParameters {	
	enum {
		eH2250LogicalChannelAckParameters_nonStandard = (1 << 31),
		eH2250LogicalChannelAckParameters_sessionID = (1 << 30),
		eH2250LogicalChannelAckParameters_mediaChannel = (1 << 29),
		eH2250LogicalChannelAckParameters_mediaControlChannel =
		    (1 << 28),
		eH2250LogicalChannelAckParameters_dynamicRTPPayloadType =
		    (1 << 27),
		eH2250LogicalChannelAckParameters_flowControlToZero =
		    (1 << 26),
		eH2250LogicalChannelAckParameters_portNumber = (1 << 25),
	} options;
	H245_TransportAddress mediaChannel;
	H245_TransportAddress mediaControlChannel;
} H2250LogicalChannelAckParameters;

typedef struct OpenLogicalChannelAck_forwardMultiplexAckParameters {	
	enum {
		eOpenLogicalChannelAck_forwardMultiplexAckParameters_h2250LogicalChannelAckParameters,
	} choice;
	union {
		H2250LogicalChannelAckParameters
		    h2250LogicalChannelAckParameters;
	};
} OpenLogicalChannelAck_forwardMultiplexAckParameters;

typedef struct OpenLogicalChannelAck {	
	enum {
		eOpenLogicalChannelAck_reverseLogicalChannelParameters =
		    (1 << 31),
		eOpenLogicalChannelAck_separateStack = (1 << 30),
		eOpenLogicalChannelAck_forwardMultiplexAckParameters =
		    (1 << 29),
		eOpenLogicalChannelAck_encryptionSync = (1 << 28),
	} options;
	OpenLogicalChannelAck_reverseLogicalChannelParameters
	    reverseLogicalChannelParameters;
	NetworkAccessParameters separateStack;
	OpenLogicalChannelAck_forwardMultiplexAckParameters
	    forwardMultiplexAckParameters;
} OpenLogicalChannelAck;

typedef struct ResponseMessage {	
	enum {
		eResponseMessage_nonStandard,
		eResponseMessage_masterSlaveDeterminationAck,
		eResponseMessage_masterSlaveDeterminationReject,
		eResponseMessage_terminalCapabilitySetAck,
		eResponseMessage_terminalCapabilitySetReject,
		eResponseMessage_openLogicalChannelAck,
		eResponseMessage_openLogicalChannelReject,
		eResponseMessage_closeLogicalChannelAck,
		eResponseMessage_requestChannelCloseAck,
		eResponseMessage_requestChannelCloseReject,
		eResponseMessage_multiplexEntrySendAck,
		eResponseMessage_multiplexEntrySendReject,
		eResponseMessage_requestMultiplexEntryAck,
		eResponseMessage_requestMultiplexEntryReject,
		eResponseMessage_requestModeAck,
		eResponseMessage_requestModeReject,
		eResponseMessage_roundTripDelayResponse,
		eResponseMessage_maintenanceLoopAck,
		eResponseMessage_maintenanceLoopReject,
		eResponseMessage_communicationModeResponse,
		eResponseMessage_conferenceResponse,
		eResponseMessage_multilinkResponse,
		eResponseMessage_logicalChannelRateAcknowledge,
		eResponseMessage_logicalChannelRateReject,
	} choice;
	union {
		OpenLogicalChannelAck openLogicalChannelAck;
	};
} ResponseMessage;

typedef struct MultimediaSystemControlMessage {	
	enum {
		eMultimediaSystemControlMessage_request,
		eMultimediaSystemControlMessage_response,
		eMultimediaSystemControlMessage_command,
		eMultimediaSystemControlMessage_indication,
	} choice;
	union {
		RequestMessage request;
		ResponseMessage response;
	};
} MultimediaSystemControlMessage;

typedef struct H323_UU_PDU_h245Control {	
	int count;
	MultimediaSystemControlMessage item[4];
} H323_UU_PDU_h245Control;

typedef struct H323_UU_PDU {	
	enum {
		eH323_UU_PDU_nonStandardData = (1 << 31),
		eH323_UU_PDU_h4501SupplementaryService = (1 << 30),
		eH323_UU_PDU_h245Tunneling = (1 << 29),
		eH323_UU_PDU_h245Control = (1 << 28),
		eH323_UU_PDU_nonStandardControl = (1 << 27),
		eH323_UU_PDU_callLinkage = (1 << 26),
		eH323_UU_PDU_tunnelledSignallingMessage = (1 << 25),
		eH323_UU_PDU_provisionalRespToH245Tunneling = (1 << 24),
		eH323_UU_PDU_stimulusControl = (1 << 23),
		eH323_UU_PDU_genericData = (1 << 22),
	} options;
	H323_UU_PDU_h323_message_body h323_message_body;
	H323_UU_PDU_h245Control h245Control;
} H323_UU_PDU;

typedef struct H323_UserInformation {	
	enum {
		eH323_UserInformation_user_data = (1 << 31),
	} options;
	H323_UU_PDU h323_uu_pdu;
} H323_UserInformation;

typedef struct GatekeeperRequest {	
	enum {
		eGatekeeperRequest_nonStandardData = (1 << 31),
		eGatekeeperRequest_gatekeeperIdentifier = (1 << 30),
		eGatekeeperRequest_callServices = (1 << 29),
		eGatekeeperRequest_endpointAlias = (1 << 28),
		eGatekeeperRequest_alternateEndpoints = (1 << 27),
		eGatekeeperRequest_tokens = (1 << 26),
		eGatekeeperRequest_cryptoTokens = (1 << 25),
		eGatekeeperRequest_authenticationCapability = (1 << 24),
		eGatekeeperRequest_algorithmOIDs = (1 << 23),
		eGatekeeperRequest_integrity = (1 << 22),
		eGatekeeperRequest_integrityCheckValue = (1 << 21),
		eGatekeeperRequest_supportsAltGK = (1 << 20),
		eGatekeeperRequest_featureSet = (1 << 19),
		eGatekeeperRequest_genericData = (1 << 18),
	} options;
	TransportAddress rasAddress;
} GatekeeperRequest;

typedef struct GatekeeperConfirm {	
	enum {
		eGatekeeperConfirm_nonStandardData = (1 << 31),
		eGatekeeperConfirm_gatekeeperIdentifier = (1 << 30),
		eGatekeeperConfirm_alternateGatekeeper = (1 << 29),
		eGatekeeperConfirm_authenticationMode = (1 << 28),
		eGatekeeperConfirm_tokens = (1 << 27),
		eGatekeeperConfirm_cryptoTokens = (1 << 26),
		eGatekeeperConfirm_algorithmOID = (1 << 25),
		eGatekeeperConfirm_integrity = (1 << 24),
		eGatekeeperConfirm_integrityCheckValue = (1 << 23),
		eGatekeeperConfirm_featureSet = (1 << 22),
		eGatekeeperConfirm_genericData = (1 << 21),
	} options;
	TransportAddress rasAddress;
} GatekeeperConfirm;

typedef struct RegistrationRequest_callSignalAddress {	
	int count;
	TransportAddress item[10];
} RegistrationRequest_callSignalAddress;

typedef struct RegistrationRequest_rasAddress {	
	int count;
	TransportAddress item[10];
} RegistrationRequest_rasAddress;

typedef struct RegistrationRequest {	
	enum {
		eRegistrationRequest_nonStandardData = (1 << 31),
		eRegistrationRequest_terminalAlias = (1 << 30),
		eRegistrationRequest_gatekeeperIdentifier = (1 << 29),
		eRegistrationRequest_alternateEndpoints = (1 << 28),
		eRegistrationRequest_timeToLive = (1 << 27),
		eRegistrationRequest_tokens = (1 << 26),
		eRegistrationRequest_cryptoTokens = (1 << 25),
		eRegistrationRequest_integrityCheckValue = (1 << 24),
		eRegistrationRequest_keepAlive = (1 << 23),
		eRegistrationRequest_endpointIdentifier = (1 << 22),
		eRegistrationRequest_willSupplyUUIEs = (1 << 21),
		eRegistrationRequest_maintainConnection = (1 << 20),
		eRegistrationRequest_alternateTransportAddresses = (1 << 19),
		eRegistrationRequest_additiveRegistration = (1 << 18),
		eRegistrationRequest_terminalAliasPattern = (1 << 17),
		eRegistrationRequest_supportsAltGK = (1 << 16),
		eRegistrationRequest_usageReportingCapability = (1 << 15),
		eRegistrationRequest_multipleCalls = (1 << 14),
		eRegistrationRequest_supportedH248Packages = (1 << 13),
		eRegistrationRequest_callCreditCapability = (1 << 12),
		eRegistrationRequest_capacityReportingCapability = (1 << 11),
		eRegistrationRequest_capacity = (1 << 10),
		eRegistrationRequest_featureSet = (1 << 9),
		eRegistrationRequest_genericData = (1 << 8),
	} options;
	RegistrationRequest_callSignalAddress callSignalAddress;
	RegistrationRequest_rasAddress rasAddress;
	unsigned int timeToLive;
} RegistrationRequest;

typedef struct RegistrationConfirm_callSignalAddress {	
	int count;
	TransportAddress item[10];
} RegistrationConfirm_callSignalAddress;

typedef struct RegistrationConfirm {	
	enum {
		eRegistrationConfirm_nonStandardData = (1 << 31),
		eRegistrationConfirm_terminalAlias = (1 << 30),
		eRegistrationConfirm_gatekeeperIdentifier = (1 << 29),
		eRegistrationConfirm_alternateGatekeeper = (1 << 28),
		eRegistrationConfirm_timeToLive = (1 << 27),
		eRegistrationConfirm_tokens = (1 << 26),
		eRegistrationConfirm_cryptoTokens = (1 << 25),
		eRegistrationConfirm_integrityCheckValue = (1 << 24),
		eRegistrationConfirm_willRespondToIRR = (1 << 23),
		eRegistrationConfirm_preGrantedARQ = (1 << 22),
		eRegistrationConfirm_maintainConnection = (1 << 21),
		eRegistrationConfirm_serviceControl = (1 << 20),
		eRegistrationConfirm_supportsAdditiveRegistration = (1 << 19),
		eRegistrationConfirm_terminalAliasPattern = (1 << 18),
		eRegistrationConfirm_supportedPrefixes = (1 << 17),
		eRegistrationConfirm_usageSpec = (1 << 16),
		eRegistrationConfirm_featureServerAlias = (1 << 15),
		eRegistrationConfirm_capacityReportingSpec = (1 << 14),
		eRegistrationConfirm_featureSet = (1 << 13),
		eRegistrationConfirm_genericData = (1 << 12),
	} options;
	RegistrationConfirm_callSignalAddress callSignalAddress;
	unsigned int timeToLive;
} RegistrationConfirm;

typedef struct UnregistrationRequest_callSignalAddress {	
	int count;
	TransportAddress item[10];
} UnregistrationRequest_callSignalAddress;

typedef struct UnregistrationRequest {	
	enum {
		eUnregistrationRequest_endpointAlias = (1 << 31),
		eUnregistrationRequest_nonStandardData = (1 << 30),
		eUnregistrationRequest_endpointIdentifier = (1 << 29),
		eUnregistrationRequest_alternateEndpoints = (1 << 28),
		eUnregistrationRequest_gatekeeperIdentifier = (1 << 27),
		eUnregistrationRequest_tokens = (1 << 26),
		eUnregistrationRequest_cryptoTokens = (1 << 25),
		eUnregistrationRequest_integrityCheckValue = (1 << 24),
		eUnregistrationRequest_reason = (1 << 23),
		eUnregistrationRequest_endpointAliasPattern = (1 << 22),
		eUnregistrationRequest_supportedPrefixes = (1 << 21),
		eUnregistrationRequest_alternateGatekeeper = (1 << 20),
		eUnregistrationRequest_genericData = (1 << 19),
	} options;
	UnregistrationRequest_callSignalAddress callSignalAddress;
} UnregistrationRequest;

typedef struct AdmissionRequest {	
	enum {
		eAdmissionRequest_callModel = (1 << 31),
		eAdmissionRequest_destinationInfo = (1 << 30),
		eAdmissionRequest_destCallSignalAddress = (1 << 29),
		eAdmissionRequest_destExtraCallInfo = (1 << 28),
		eAdmissionRequest_srcCallSignalAddress = (1 << 27),
		eAdmissionRequest_nonStandardData = (1 << 26),
		eAdmissionRequest_callServices = (1 << 25),
		eAdmissionRequest_canMapAlias = (1 << 24),
		eAdmissionRequest_callIdentifier = (1 << 23),
		eAdmissionRequest_srcAlternatives = (1 << 22),
		eAdmissionRequest_destAlternatives = (1 << 21),
		eAdmissionRequest_gatekeeperIdentifier = (1 << 20),
		eAdmissionRequest_tokens = (1 << 19),
		eAdmissionRequest_cryptoTokens = (1 << 18),
		eAdmissionRequest_integrityCheckValue = (1 << 17),
		eAdmissionRequest_transportQOS = (1 << 16),
		eAdmissionRequest_willSupplyUUIEs = (1 << 15),
		eAdmissionRequest_callLinkage = (1 << 14),
		eAdmissionRequest_gatewayDataRate = (1 << 13),
		eAdmissionRequest_capacity = (1 << 12),
		eAdmissionRequest_circuitInfo = (1 << 11),
		eAdmissionRequest_desiredProtocols = (1 << 10),
		eAdmissionRequest_desiredTunnelledProtocol = (1 << 9),
		eAdmissionRequest_featureSet = (1 << 8),
		eAdmissionRequest_genericData = (1 << 7),
	} options;
	TransportAddress destCallSignalAddress;
	TransportAddress srcCallSignalAddress;
} AdmissionRequest;

typedef struct AdmissionConfirm {	
	enum {
		eAdmissionConfirm_irrFrequency = (1 << 31),
		eAdmissionConfirm_nonStandardData = (1 << 30),
		eAdmissionConfirm_destinationInfo = (1 << 29),
		eAdmissionConfirm_destExtraCallInfo = (1 << 28),
		eAdmissionConfirm_destinationType = (1 << 27),
		eAdmissionConfirm_remoteExtensionAddress = (1 << 26),
		eAdmissionConfirm_alternateEndpoints = (1 << 25),
		eAdmissionConfirm_tokens = (1 << 24),
		eAdmissionConfirm_cryptoTokens = (1 << 23),
		eAdmissionConfirm_integrityCheckValue = (1 << 22),
		eAdmissionConfirm_transportQOS = (1 << 21),
		eAdmissionConfirm_willRespondToIRR = (1 << 20),
		eAdmissionConfirm_uuiesRequested = (1 << 19),
		eAdmissionConfirm_language = (1 << 18),
		eAdmissionConfirm_alternateTransportAddresses = (1 << 17),
		eAdmissionConfirm_useSpecifiedTransport = (1 << 16),
		eAdmissionConfirm_circuitInfo = (1 << 15),
		eAdmissionConfirm_usageSpec = (1 << 14),
		eAdmissionConfirm_supportedProtocols = (1 << 13),
		eAdmissionConfirm_serviceControl = (1 << 12),
		eAdmissionConfirm_multipleCalls = (1 << 11),
		eAdmissionConfirm_featureSet = (1 << 10),
		eAdmissionConfirm_genericData = (1 << 9),
	} options;
	TransportAddress destCallSignalAddress;
} AdmissionConfirm;

typedef struct LocationRequest {	
	enum {
		eLocationRequest_endpointIdentifier = (1 << 31),
		eLocationRequest_nonStandardData = (1 << 30),
		eLocationRequest_sourceInfo = (1 << 29),
		eLocationRequest_canMapAlias = (1 << 28),
		eLocationRequest_gatekeeperIdentifier = (1 << 27),
		eLocationRequest_tokens = (1 << 26),
		eLocationRequest_cryptoTokens = (1 << 25),
		eLocationRequest_integrityCheckValue = (1 << 24),
		eLocationRequest_desiredProtocols = (1 << 23),
		eLocationRequest_desiredTunnelledProtocol = (1 << 22),
		eLocationRequest_featureSet = (1 << 21),
		eLocationRequest_genericData = (1 << 20),
		eLocationRequest_hopCount = (1 << 19),
		eLocationRequest_circuitInfo = (1 << 18),
	} options;
	TransportAddress replyAddress;
} LocationRequest;

typedef struct LocationConfirm {	
	enum {
		eLocationConfirm_nonStandardData = (1 << 31),
		eLocationConfirm_destinationInfo = (1 << 30),
		eLocationConfirm_destExtraCallInfo = (1 << 29),
		eLocationConfirm_destinationType = (1 << 28),
		eLocationConfirm_remoteExtensionAddress = (1 << 27),
		eLocationConfirm_alternateEndpoints = (1 << 26),
		eLocationConfirm_tokens = (1 << 25),
		eLocationConfirm_cryptoTokens = (1 << 24),
		eLocationConfirm_integrityCheckValue = (1 << 23),
		eLocationConfirm_alternateTransportAddresses = (1 << 22),
		eLocationConfirm_supportedProtocols = (1 << 21),
		eLocationConfirm_multipleCalls = (1 << 20),
		eLocationConfirm_featureSet = (1 << 19),
		eLocationConfirm_genericData = (1 << 18),
		eLocationConfirm_circuitInfo = (1 << 17),
		eLocationConfirm_serviceControl = (1 << 16),
	} options;
	TransportAddress callSignalAddress;
	TransportAddress rasAddress;
} LocationConfirm;

typedef struct InfoRequestResponse_callSignalAddress {	
	int count;
	TransportAddress item[10];
} InfoRequestResponse_callSignalAddress;

typedef struct InfoRequestResponse {	
	enum {
		eInfoRequestResponse_nonStandardData = (1 << 31),
		eInfoRequestResponse_endpointAlias = (1 << 30),
		eInfoRequestResponse_perCallInfo = (1 << 29),
		eInfoRequestResponse_tokens = (1 << 28),
		eInfoRequestResponse_cryptoTokens = (1 << 27),
		eInfoRequestResponse_integrityCheckValue = (1 << 26),
		eInfoRequestResponse_needResponse = (1 << 25),
		eInfoRequestResponse_capacity = (1 << 24),
		eInfoRequestResponse_irrStatus = (1 << 23),
		eInfoRequestResponse_unsolicited = (1 << 22),
		eInfoRequestResponse_genericData = (1 << 21),
	} options;
	TransportAddress rasAddress;
	InfoRequestResponse_callSignalAddress callSignalAddress;
} InfoRequestResponse;

typedef struct RasMessage {	
	enum {
		eRasMessage_gatekeeperRequest,
		eRasMessage_gatekeeperConfirm,
		eRasMessage_gatekeeperReject,
		eRasMessage_registrationRequest,
		eRasMessage_registrationConfirm,
		eRasMessage_registrationReject,
		eRasMessage_unregistrationRequest,
		eRasMessage_unregistrationConfirm,
		eRasMessage_unregistrationReject,
		eRasMessage_admissionRequest,
		eRasMessage_admissionConfirm,
		eRasMessage_admissionReject,
		eRasMessage_bandwidthRequest,
		eRasMessage_bandwidthConfirm,
		eRasMessage_bandwidthReject,
		eRasMessage_disengageRequest,
		eRasMessage_disengageConfirm,
		eRasMessage_disengageReject,
		eRasMessage_locationRequest,
		eRasMessage_locationConfirm,
		eRasMessage_locationReject,
		eRasMessage_infoRequest,
		eRasMessage_infoRequestResponse,
		eRasMessage_nonStandardMessage,
		eRasMessage_unknownMessageResponse,
		eRasMessage_requestInProgress,
		eRasMessage_resourcesAvailableIndicate,
		eRasMessage_resourcesAvailableConfirm,
		eRasMessage_infoRequestAck,
		eRasMessage_infoRequestNak,
		eRasMessage_serviceControlIndication,
		eRasMessage_serviceControlResponse,
	} choice;
	union {
		GatekeeperRequest gatekeeperRequest;
		GatekeeperConfirm gatekeeperConfirm;
		RegistrationRequest registrationRequest;
		RegistrationConfirm registrationConfirm;
		UnregistrationRequest unregistrationRequest;
		AdmissionRequest admissionRequest;
		AdmissionConfirm admissionConfirm;
		LocationRequest locationRequest;
		LocationConfirm locationConfirm;
		InfoRequestResponse infoRequestResponse;
	};
} RasMessage;

#endif 
