import https from 'https';
import http from 'http';
import got from 'got';
import { logger } from './logger';

function download(url: string, w: any) {
    logger.info(`Downloading from: ${url}`);
    return new Promise((resolve, reject) => {
        let protocol = /^https:/.exec(url) ? https : http;
        protocol
            .get(url, (res1: any) => {
                protocol = /^https:/.exec(res1.headers.location) ? https : http;
                protocol
                    .get(res1.headers.location, (res2: any) => {
                        res2.pipe(w);
                        res2.on('error', reject);
                        res2.on('end', resolve);
                    })
                    .on('error', reject);
            })
            .on('error', reject);
    });
}

function sendRequest(url: string) {
    return new Promise((resolve, reject) => {
        logger.info(`Sending request to: ${url}`);
        const resp = got.get(url, { headers: { Accept: 'application/vnd.github.v3+json' } }); // send request to github, returns a promise
        // handle promises
        Promise.all([resp])
            .then((results) => {
                results.forEach((result) => {
                    const body = JSON.parse(result.body);
                    resolve(body);
                });
            })
            .catch((reason) => {
                reject(reason);
            });
    });
}

function getReleaseList(body: any) {
    const releases: any[] = []; // declare array
    body.forEach((release: any) => {
        releases.push({
            url: release.url,
            tag: release.tag_name,
            created: release.created_at,
            published: release.published_at,
            assets: [],
        });
        releases.sort((a: any, b: any) => {
            return new Date(b.created).getTime() - new Date(a.created).getTime();
        });
    });
    return releases;
}

function getLatestRelease(url: string) {
    return new Promise((resolve, reject) => {
        const promise = sendRequest(url);
        let latest: any;
        Promise.all([promise])
            .then((results) => {
                results.forEach((result) => {
                    latest = getReleaseList(result)[0];
                    const promise = sendRequest(latest.url);
                    Promise.all([promise])
                        .then((results) => {
                            results.forEach((result: any) => {
                                result.assets.forEach((asset: any) => {
                                    latest.assets.push({
                                        browserDownloadUrl: asset.browser_download_url,
                                        name: asset.name,
                                    });
                                });
                                resolve(latest);
                            });
                        })
                        .catch((reason) => {
                            reject(reason);
                        });
                });
            })
            .catch((reason) => {
                reject(reason);
            });
    });
}

export { getLatestRelease, getReleaseList, download };
