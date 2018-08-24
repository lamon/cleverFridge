package com.cleverfridge

import groovy.util.logging.Slf4j
import io.micronaut.runtime.Micronaut
import groovy.transform.CompileStatic

import javax.inject.Singleton

@CompileStatic
@Singleton
@Slf4j
class Application {
    static void main(String[] args) {
        Micronaut.run(Application)
    }
}