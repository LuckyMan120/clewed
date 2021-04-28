<?php
namespace Clewed\Insights;

/**
 * Helper functions related to insights
 *
 * @author Dmytro Vovk <dmitry.vovk@gmail.com>
 */
class InsightHelper {

    /**
     * Tells if user seen this insight id and maintains cookie for that
     *
     * @param int $id
     *
     * @return bool
     */
    public static function seenInsight($id) {
        $cookieKey = 'ci_' . md5('clewed_seen_insight');
        $ids = array($id);
        $seen = false;
        if (!empty($_COOKIE[$cookieKey])) {
            $decoded = base64_decode($_COOKIE[$cookieKey]);
            if (!empty($decoded)) {
                $values = explode('|', $decoded);
                if (is_array($values)) {
                    $seen = false !== array_search($id, $values);
                    $ids = array_unique(array_merge($ids, $values));
                }
            }
        }
        $payload = base64_encode(implode('|', $ids));
        $file = '';
        $line = 0;
        if (!headers_sent($file, $line)) {
            setcookie($cookieKey, $payload, time() + 60 * 60 * 24 * 60);
        } else {
            error_log(sprintf('Could not set cookie in %s:%d, headers already sent in %s:%d', __FILE__, __LINE__, $file, $line));
        }
        return $seen;
    }
}
