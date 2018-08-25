package com.cleverfridge

import com.cleverfridge.clients.DweetClient
import com.cleverfridge.clients.NexmoClient
import com.cleverfridge.domain.DoorOpenedCount
import com.cleverfridge.domain.NexmoSMS
import groovy.time.TimeCategory
import groovy.transform.CompileDynamic
import groovy.transform.CompileStatic
import groovy.util.logging.Slf4j
import io.micronaut.context.annotation.Value

import javax.inject.Singleton
import java.text.SimpleDateFormat

@Slf4j
@CompileStatic
@Singleton
class LuminosityUseCase {

    @Value('${nexmo.api.key}')
    String api_key

    @Value('${nexmo.api.secret}')
    String api_secret

    @Value('${nexmo.phone.number.to}')
    String phoneNumberTo

    @Value('${fridge.sensor.luminosity.threshold}')
    Long luminosityThreshold

    @Value('${fridge.sensor.luminosity.minutes.threshold}')
    Long luminosityMinutesThreshold

    Long currentLuminosity
    Date currentLuminosityDate

    Long lastLuminosity
    Date lastLuminosityDate

    Date fridgeDoorOpeningDate
    Long doorOpenedCount = 0

    final DweetClient dweetClient
    final NexmoClient nexmoClient

    LuminosityUseCase(DweetClient dweetClient, NexmoClient nexmoClient) {
        this.dweetClient = dweetClient
        this.nexmoClient = nexmoClient
    }

    @CompileDynamic
    void handleLuminosity(String luminosity) {

        currentLuminosity = Long.valueOf(luminosity)
        currentLuminosityDate = new Date()

        if (lastLuminosity && lastLuminosityDate) {
            log.info("lastLuminosity: {}, lastLuminosityDate: {},  currentLuminosity: {}, currentLuminosityDate: {}",
                    lastLuminosity, new SimpleDateFormat("dd/MM/yyyy hh:mm:ss").format(lastLuminosityDate),
                    currentLuminosity, new SimpleDateFormat("dd/MM/yyyy hh:mm:ss").format(currentLuminosityDate))
        }

        if (currentLuminosity > luminosityThreshold) {

            log.info('tem luz, porta da geladeira aberta ...')

            if (!fridgeDoorOpeningDate) {
                fridgeDoorOpeningDate = currentLuminosityDate
                incrementDoorOpenedCountAtDweet()
            }

            if (lastLuminosityDate) {

                use(TimeCategory) {
                    def numberOfMinutes = (lastLuminosityDate - fridgeDoorOpeningDate).minutes

                    log.info('lastLuminosityDate: {}, fridgeDoorOpeningDate: {}, numberOfMinutes: {}',
                            lastLuminosityDate, fridgeDoorOpeningDate, numberOfMinutes)

                    if (numberOfMinutes >= luminosityMinutesThreshold) {

                        log.info('porta da geladeira aberta por muito tempo... enviando SMS')

                        sendSMS()

                        fridgeDoorOpeningDate = null

                    }

                }

            }

        } else {
            fridgeDoorOpeningDate = null
        }

        lastLuminosity = currentLuminosity
        lastLuminosityDate = currentLuminosityDate

    }

    private void incrementDoorOpenedCountAtDweet() {

        log.info('incrementando contador de porta aberta ...')

        sleep(1000)

        doorOpenedCount++

        def doorOpenedCountInstance = new DoorOpenedCount(
                doorOpenedCount: doorOpenedCount.toString()
        )

        dweetClient.incrementDoorOpenedCount(doorOpenedCountInstance)
    }

    private void sendSMS() {
        def nexmoSMS = new NexmoSMS(api_key: api_key,
                                    api_secret: api_secret,
                                    to: phoneNumberTo,
                                    from: "Clever Fridge",
                                    text: "A porta da geladeira está aberta por muito tempo!")

        nexmoClient.sendSMS(nexmoSMS)
    }

}
