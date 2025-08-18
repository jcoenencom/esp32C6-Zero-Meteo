import * as m from "zigbee-herdsman-converters/lib/modernExtend";
const defaultReporting = {min: 0, max: 3600, change: 0};

export default {
    zigbeeModel: ["ZigbeeMeteoStation"],
    model: "ZigbeeMeteoStation",
    vendor: "ON4KCH",
    description: "Automatically generated definition",
    extend: [
        m.deviceEndpoints({endpoints: {10: 10, 11: 11, 12: 12, 9: 9}}),
        m.temperature({endpointNames: ["10"]}),
        m.humidity({endpointNames: ["10"]}),
        m.pressure({endpointNames: ["11"]}),
        m.numeric({
            name: "Wind_Speed",
            unit: "km/hr",
            scale: 1,
            cluster: "genAnalogInput",
            attribute: "presentValue",
            description: "Current wind speed in km/hr",
            access: "STATE",
            endpointNames: ["12"],
            precision: 2,
            reporting: defaultReporting,
        }),
        m.numeric({
            name: "Wind_Gust",
            unit: "km/hr",
            scale: 1,
            cluster: "genAnalogInput",
            attribute: "presentValue",
            description: "Current wind gust speed over 5 min (km/hr)",
            access: "STATE",
            endpointNames: ["9"],
            precision: 2,
            reporting: defaultReporting,
        }),
    ],
    meta: {multiEndpoint: true, friendly_name: "ON4KCH's WS"},
};
