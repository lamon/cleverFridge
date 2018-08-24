package com.cleverfridge.clients

import com.cleverfridge.domain.IFTTTValues
import groovy.transform.CompileStatic
import io.micronaut.http.annotation.Body
import io.micronaut.http.annotation.Post
import io.micronaut.http.client.Client

@CompileStatic
@Client('${ifttt.url}')
interface IFTTTClient {

    @Post('/trigger/${ifttt.email.event}/with/key/${ifttt.key}')
    void triggerEmailEvent(@Body IFTTTValues iftttValues)

}
