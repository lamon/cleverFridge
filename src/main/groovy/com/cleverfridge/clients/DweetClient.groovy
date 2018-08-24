package com.cleverfridge.clients

import com.cleverfridge.domain.DoorOpenedCount
import com.cleverfridge.domain.FridgeSensor
import groovy.transform.CompileStatic
import io.micronaut.http.annotation.Body
import io.micronaut.http.annotation.Post
import io.micronaut.http.client.Client

@CompileStatic
@Client('${dweet.url}')
interface DweetClient {

    @Post('/dweet/for/${dweet.thing.name}')
    void createDweet(@Body FridgeSensor fridgeSensor)

    @Post('/dweet/for/${dweet.thing.name}')
    void incrementDoorOpenedCount(@Body DoorOpenedCount doorOpenedCount)

}
