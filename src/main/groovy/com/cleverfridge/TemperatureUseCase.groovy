package com.cleverfridge


import com.cleverfridge.clients.IFTTTClient
import com.cleverfridge.domain.IFTTTValues
import groovy.transform.CompileStatic
import groovy.util.logging.Slf4j
import io.micronaut.context.annotation.Value

import javax.inject.Singleton
import java.text.SimpleDateFormat

@Slf4j
@CompileStatic
@Singleton
class TemperatureUseCase {

    @Value('${fridge.sensor.temperature.threshold}')
    protected Long temperatureThreshold

    Long currentTemperature
    Date currentTemperatureDate

    final IFTTTClient iftttClient

    TemperatureUseCase(IFTTTClient iftttClient) {
        this.iftttClient = iftttClient
    }

    void handleTemperature(String temperature) {

        currentTemperature = Long.valueOf(temperature)
        currentTemperatureDate = new Date()

        log.info("currentTemperature: {}, currentTemperatureDate: {}",
                 currentTemperature, new SimpleDateFormat("dd/MM/yyyy hh:mm:ss").format(currentTemperatureDate))

        if (currentTemperature > temperatureThreshold) {

            log.info('temperatura aumentou muito, possivel queda de energia... enviando email')

            def iftttValues = new IFTTTValues(value1: currentTemperature.toString())
            iftttClient.triggerEmailEvent(iftttValues)

        }

    }

}
