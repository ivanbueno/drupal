<?php

namespace Drupal\epm\Service;

use Drupal\Component\Utility\Html;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Component\Serialization\Json;
use GuzzleHttp\ClientInterface;
use GuzzleHttp\RequestOptions;

/**
 * Defines GoogleVisionApi service.
 */
class GoogleLanguageApi implements GoogleLanguageApiInterface {

    /**
     * The base url of the Google Cloud Vision API.
     */
    const APIEndpoint = 'https://language.googleapis.com/v1/documents:';

    /**
     * The config factory.
     *
     * @var \Drupal\Core\Config\ConfigFactoryInterface
     */
    protected $configFactory;

    /**
     * The HTTP client to fetch the feed data with.
     *
     * @var \GuzzleHttp\ClientInterface
     */
    protected $httpClient;

    /**
     * Stores the API key.
     *
     * @var int
     */
    protected $apiKey;

    /**
     * Stores the maxResults number for the LABEL_DETECTION feature.
     *
     * @var int
     */
    protected $maxResultsLabelDetection;

    /**
     * Construct a GoogleVisionApi object.
     *
     * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
     *   The config factory.
     *
     * @param \GuzzleHttp\ClientInterface $http_client
     *   A Guzzle client object.
     */
    public function __construct(ConfigFactoryInterface $config_factory, ClientInterface $http_client) {
        $this->configFactory = $config_factory;
        $this->httpClient = $http_client;
        $this->apiKey = $this->configFactory->get('epm.google_vision_settings')
            ->get('api_key');
        $this->maxResultsLabelDetection = $this->configFactory->get('epm.google_vision_settings')
            ->get('max_results_labels');
    }

    /**
     * Function to make request through httpClient service.
     *
     * @param $function
     *  The resource function to call.
     *
     * @param $data
     *  The object to be passed during the API call.
     *
     * @return array
     * An array obtained in response from the API call.
     */
    protected function postRequest($function, $data) {
        $url = static::APIEndpoint . $function . '?key=' . $this->apiKey;
        try {
            $response = $this->httpClient->post($url, [
                RequestOptions::JSON => $data,
                RequestOptions::HEADERS => ['Content-Type' => 'application/json'],
            ]);
            return Json::decode($response->getBody());
        }
        catch (\Exception $e) {
            if ($e->getCode() == '403') {
                drupal_set_message(t('The Google Vision API could not be successfully reached. Please check your API credentials and try again. The raw error message from the server is shown below.'), 'warning');
            }
            drupal_set_message(Html::escape($e->getMessage()), 'warning');
            return ['error' => $e->getMessage()];
        }
    }

    /**
     * {@inheritdoc}
     */
    public function analyzeEntities($text) {
        if (!$this->apiKey) {
            return FALSE;
        }

        // Prepare JSON.
        $data = [
            'document' => [
                'type' => 'PLAIN_TEXT',
                'language' => 'EN',
                'content' => $text
            ],
            'encodingType' => 'UTF8',
        ];

        $result = $this->postRequest(__FUNCTION__, $data);
        if (empty($result['error'])) {
            return $result;
        }
        return FALSE;

    }
}
