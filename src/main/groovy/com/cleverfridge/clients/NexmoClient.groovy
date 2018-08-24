package com.cleverfridge.clients

import com.cleverfridge.domain.NexmoSMS
import groovy.transform.CompileStatic
import io.micronaut.http.annotation.Body
import io.micronaut.http.annotation.Post
import io.micronaut.http.client.Client

@CompileStatic
@Client('${nexmo.url}')
interface NexmoClient {

    @Post('/sms/json')
    void sendSMS(@Body NexmoSMS nexmoSMS)

}
