package com.cleverfridge.domain

import io.micronaut.context.annotation.Value

class NexmoSMS {

    String api_key
    String api_secret
    String to
    String from
    String text

}