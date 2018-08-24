package com.cleverfridge

import groovy.transform.CompileStatic
import groovy.util.logging.Slf4j
import io.micronaut.runtime.Micronaut

import javax.inject.Singleton

@CompileStatic
@Singleton
@Slf4j
class Application {
    static void main(String[] args) {
        Micronaut.run(Application)
    }
}