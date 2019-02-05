<?php

namespace Drupal\epm\Service;

/**
 * Interface for GoogleVisionApi.
 */
interface GoogleLanguageApiInterface {

    /**
     * Finds named entities (currently proper names and common nouns) in the text along with entity types, salience,
     * mentions for each entity, and other properties.
     *
     * @param string $text .
     *
     * @return Array|bool.
     */
    public function analyzeEntities($text);

}
