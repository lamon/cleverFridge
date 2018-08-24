package com.cleverfridge

import com.cleverfridge.clients.DweetClient
import com.cleverfridge.domain.FridgeSensor
import groovy.util.logging.Slf4j
import io.micronaut.http.HttpResponse
import io.micronaut.http.annotation.Body
import io.micronaut.http.annotation.Controller
import io.micronaut.http.annotation.Post

@Controller("/cleverFridge")
@Slf4j
class CleverFridgeController {

    final LuminosityUseCase luminosityUseCase
    final TemperatureUseCase temperatureUseCase
    final DweetClient dweetClient

    CleverFridgeController(LuminosityUseCase luminosityUseCase,
                           TemperatureUseCase temperatureUseCase,
                           DweetClient dweetClient) {
        this.luminosityUseCase = luminosityUseCase
        this.temperatureUseCase = temperatureUseCase
        this.dweetClient = dweetClient
    }

    @Post('/')
    HttpResponse save(@Body FridgeSensor fridgeSensor) {

        createDweet(fridgeSensor)

        luminosityUseCase.handleLuminosity(fridgeSensor.luminosity)
        temperatureUseCase.handleTemperature(fridgeSensor.temperature)

        HttpResponse.ok()
    }

    private createDweet(FridgeSensor fridgeSensor) {

        log.info('enviando dados do sensores para o Dweet ...')

        dweetClient.createDweet(fridgeSensor)
    }
    
}
